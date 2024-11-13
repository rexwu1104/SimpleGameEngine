#ifndef FORMAT_H
#define FORMAT_H

#define byte unsigned char

#include <string>
#include <vector>
#include <map>

namespace literals {
    enum Ansi {
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White
    };

    enum Position {
        Foreground = 3,
        Background
    };

    enum Mode {
        None,
        Bits3_4,
        Bits8,
        Bits24,
    };
}

enum Level {
    Basic = 0,
    Bold = 1,
    Underline = 2,
    Italic = 4,
    Overline = 8,
    Fg = 16,
    Bg = 32
};

class FormatString {
private:
    std::vector<std::pair<size_t, size_t>> style_ranges;
    std::vector<std::pair<size_t, std::string>> styles;
    std::string content;
protected:
    void append_style(size_t, size_t, const std::string&, Level);
public:
    explicit FormatString(const std::string&& content) : content(content) {};
    explicit FormatString(const char* content) : content(content) {};

    FormatString& append(const std::string&&);
    FormatString& operator+(const FormatString&);

    explicit operator std::string() const;
    friend std::ostream& operator<<(std::ostream&, const FormatString&);

    friend class FormatBase;
};

FormatString operator""_ansi(const char*, size_t);

class LazyMode {
private:
    std::map<std::ostream*, literals::Mode> stream_table;
    LazyMode() = default;
public:
    LazyMode(const LazyMode&&) = delete;

    static LazyMode& get();
    literals::Mode get_value(std::ostream* = nullptr);
};

class FormatBase {
private:
    std::string prefix;
    Level level = Basic;
protected:
    void set_prefix(const std::string&&);
    void set_level(const Level&&);
public:
    virtual ~FormatBase() = default;

    FormatString& apply(FormatString&) const;
    FormatString& apply(FormatString&, size_t, size_t) const;
};

class ColorFormat final : public FormatBase {
public:
    ColorFormat() = default;

    ColorFormat& set_basic(literals::Ansi, literals::Position);
    ColorFormat& set_8bit(literals::Ansi, literals::Position);
    ColorFormat& set_8bit(byte, byte, byte, literals::Position);
    ColorFormat& set_8bit(byte, literals::Position);
    ColorFormat& set_24bit(byte, byte, byte, literals::Position);
};

class FontFormat final : public FormatBase {
private:
    bool bold      = false,
         italic    = false,
         underline = false,
         overline  = false;

    void update();
public:
    FontFormat() = default;

    FontFormat& set_bold(bool);
    FontFormat& set_italic(bool);
    FontFormat& set_underline(bool);
    FontFormat& set_overline(bool);
};

#endif
