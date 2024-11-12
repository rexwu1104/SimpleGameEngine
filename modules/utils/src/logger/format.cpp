#include <algorithm>
#include <sstream>
#include <format>
#include <iostream>
#include <ranges>
#include <unistd.h>
#if defined(__WIN32)
#include <windows.h>
#define isatty _isatty
#define fileno _fileno
#endif
#include <logger/format.h>

void FormatString::append_style(size_t start, size_t end, const std::string &style, Level level) {
    style_ranges.emplace_back(start, end);
    styles.emplace_back(level, style);
}

FormatString& FormatString::append(const std::string&& s) {
    content.append(s);
    return *this;
}

FormatString operator""_ansi(const char* cstr, size_t) {
    return FormatString(cstr);
}

// todo: Here
FormatString::operator std::string() const {
    static auto reset = "\033[0m";
    std::stringstream ss;

    auto zip = std::views::zip(style_ranges, styles);
    auto iter = std::vector(zip.begin(), zip.end());
    std::ranges::sort(iter, [](const auto& a, const auto& b) {
        return a.first.first < b.first.first;
    });

    for (const auto& [range, style_info]: iter) {
        const auto [start, end] = range;
        const auto [level, style_] = style_info;
        std::cout << std::format("[ [{}, {}], [{}, {}] ]", start, end, level, style_);
    }

    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const FormatString& format) {
    return os << std::string(format);
}

FormatString& FormatString::operator+(const FormatString &other) {
    const auto length = content.length();
    for (const auto&[start, end]: other.style_ranges)
        style_ranges.emplace_back(start + length, end + length);
    std::ranges::copy(other.styles, std::back_inserter(styles));
    content.append(other.content);
    return *this;
}

LazyMode& LazyMode::get() {
    static LazyMode* instance = nullptr;
    if (!instance)
        instance = new LazyMode();
    return *instance;
}

color_literals::Mode LazyMode::get_value(std::ostream* stream_p) {
    auto result = color_literals::Mode::None;
    if (!stream_p)
        return color_literals::Mode::Bits24;
    if (stream_table.contains(stream_p))
        return stream_table[stream_p];
#if defined(__WIN32)
    auto h = INVALID_HANDLE_VALUE;
    DWORD dwMode = 0;
#endif
#define to_addr(v) reinterpret_cast<size_t>(reinterpret_cast<void*>(&(v)))
    FILE* channel = nullptr;
    const auto cout_addr = to_addr(std::cout);
    const auto clog_addr = to_addr(std::clog);
    const auto cerr_addr = to_addr(std::cerr);
    const auto stream_addr = to_addr(*stream_p);

    if (stream_addr == clog_addr || stream_addr == cout_addr) {
        channel = stdout;
#if defined(__WIN32)
        h = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
    }
    if (stream_addr == cerr_addr) {
        channel = stderr;
#if defined(__WIN32)
        h = GetStdHandle(STD_ERROR_HANDLE);
#endif
    }

    // not a standard ostream in system, like ofstream, sstream, etc.
    if (!channel) {
        result = color_literals::Mode::None;
        goto end;
    }
#undef to_addr
    if (!isatty(fileno(channel)))
        goto last_check;
#if defined(__WIN32)
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);
    if (osvi.dwMajorVersion < 10) {
        result = color_literals::Mode::Bits3_4;
        goto end;
    }

    if ((h == INVALID_HANDLE_VALUE) ||
        (!GetConsoleMode(h, &dwMode)) ||
        (!SetConsoleMode(h, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)))
        goto last_check;

    result = color_literals::Mode::Bits24;
#endif
last_check:
    if (const char* colorterm = std::getenv("COLORTERM")) {
        const std::string ct(colorterm);
        if (ct.find("truecolor") != std::string::npos ||
            ct.find("24bit") != std::string::npos)
            result = color_literals::Mode::Bits24;
        else if (ct.find("8bit") != std::string::npos)
            result = color_literals::Mode::Bits8;
    }

    if (const char* term = std::getenv("TERM")) {
        const std::string t(term);
        if (t.find("256color") != std::string::npos)
            result = color_literals::Mode::Bits8;
        else if (t.find("color") != std::string::npos)
            result = color_literals::Mode::Bits3_4;
        else result = color_literals::Mode::Bits24;
    }
end:
    return stream_table[stream_p] = result;
}

void FormatBase::set_prefix(const std::string&& s) {
    prefix = s;
}

void FormatBase::set_level(const Level&& l) {
    level = l;
}

FormatString& FormatBase::apply(FormatString &str) const {
    str.append_style(0, str.content.size(), prefix, level);
    return str;
}

FormatString &FormatBase::apply(FormatString &str, const size_t start, const size_t end) const {
    str.append_style(start, end, prefix, level);
    return str;
}

ColorFormat& ColorFormat::set_basic(const color_literals::Ansi color, const color_literals::Position pos) {
    set_level(pos == color_literals::Position::Background ? Bg : Fg);
    set_prefix(std::format(
        "\033[{}{}m",
        3 + (pos == color_literals::Position::Background),
        static_cast<size_t>(color)));
    return *this;
}

ColorFormat& ColorFormat::set_8bit(const color_literals::Ansi color, const color_literals::Position pos) {
    set_level(pos == color_literals::Position::Background ? Bg : Fg);
    set_prefix(std::format(
        "\033[{}8;5;{}m",
        3 + (pos == color_literals::Position::Background),
        static_cast<size_t>(color)));
    return *this;
}

ColorFormat& ColorFormat::set_8bit(const byte gray, const color_literals::Position pos) {
    set_level(pos == color_literals::Position::Background ? Bg : Fg);
    set_prefix(std::format(
        "\033[{}8;5;{}m",
        3 + (pos == color_literals::Position::Background),
        232 + gray));
    return *this;
}

ColorFormat& ColorFormat::set_8bit(const byte r, const byte g, const byte b, const color_literals::Position pos) {
    set_level(pos == color_literals::Position::Background ? Bg : Fg);
    set_prefix(std::format(
        "\033[{}8;5;{}m",
        3 + (pos == color_literals::Position::Background),
        16 + 36 * r + 6 * g + b));
    return *this;
}

ColorFormat& ColorFormat::set_24bit(const byte r, const byte g, const byte b, const color_literals::Position pos) {
    set_level(pos == color_literals::Position::Background ? Bg : Fg);
    set_prefix(std::format(
        "\033[{}8;5;{};{};{}m",
        3 + (pos == color_literals::Position::Background),
        r, g, b));
    return *this;
}

FontFormat& FontFormat::set_bold(const bool to_bold) {
    if (bold == to_bold) return *this;
    bold = to_bold;
    set_level(static_cast<Level>((bold ? Bold : Basic) |
                                 (italic ? Italic : Basic) |
                                 (underline ? Underline : Basic) |
                                 (overline ? Overline : Basic)));
    set_prefix(std::string() +
        (bold ? "\033[1m" : "\033[21;22m") +
        (italic ? "\033[3m" : "\033[23m") +
        (underline ? "\033[4m" : "\033[24m") +
        (overline ? "\033[53m" : "\033[55m"));
    return *this;
}

FontFormat& FontFormat::set_italic(bool to_italic) {
    if (italic == to_italic) return *this;
    italic = to_italic;
    set_level(static_cast<Level>((bold ? Bold : Basic) |
                                 (italic ? Italic : Basic) |
                                 (underline ? Underline : Basic) |
                                 (overline ? Overline : Basic)));
    set_prefix(std::string() +
        (bold ? "\033[1m" : "\033[21;22m") +
        (italic ? "\033[3m" : "\033[23m") +
        (underline ? "\033[4m" : "\033[24m") +
        (overline ? "\033[53m" : "\033[55m"));
    return *this;
}

FontFormat &FontFormat::set_underline(bool to_underline) {
    if (underline == to_underline) return *this;
    underline = to_underline;
    set_level(static_cast<Level>((bold ? Bold : Basic) |
                                 (italic ? Italic : Basic) |
                                 (underline ? Underline : Basic) |
                                 (overline ? Overline : Basic)));
    set_prefix(std::string() +
        (bold ? "\033[1m" : "\033[21;22m") +
        (italic ? "\033[3m" : "\033[23m") +
        (underline ? "\033[4m" : "\033[24m") +
        (overline ? "\033[53m" : "\033[55m"));
    return *this;
}

FontFormat& FontFormat::set_overline(bool to_overline) {
    if (overline == to_overline) return *this;
    overline = to_overline;
    set_level(static_cast<Level>((bold ? Bold : Basic) |
                                 (italic ? Italic : Basic) |
                                 (underline ? Underline : Basic) |
                                 (overline ? Overline : Basic)));
    set_prefix(std::string() +
        (bold ? "\033[1m" : "\033[21;22m") +
        (italic ? "\033[3m" : "\033[23m") +
        (underline ? "\033[4m" : "\033[24m") +
        (overline ? "\033[53m" : "\033[55m"));
    return *this;
}
