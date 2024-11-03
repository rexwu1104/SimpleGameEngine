#include <iostream>
#include <fstream>

#include <unistd.h>
#if defined(_WIN32)
#include <windows.h>
#define isatty _isatty
#define fileno _fileno
#endif

#include <logger/channel.h>
#include <logger/color.h>

LoggerChannelBase::LoggerChannelBase(const ColorFormat::EnableMode mode) :
    pipe(mode) {
}

void StdLoggerChannel::send(std::string&& message) {
    output << pipe.apply(message) << std::endl;
}

void FileLoggerChannel::send(std::string&& message) {
    output << pipe.apply(message) << std::endl;
}

ColorFormat& LoggerChannelBase::add_format() {
    return pipe.add();
}

ColorFormat& LoggerChannelBase::new_format() {
    pipe.reset();
    return pipe.add();
}

StdLoggerChannel::StdLoggerChannel(std::ostream&& stream) :
    LoggerChannelBase(enable_mode()),
    output(stream) {
}

ColorFormat::EnableMode StdLoggerChannel::enable_mode() {
    ColorFormat::EnableMode result = ColorFormat::EnableMode::Ansi;
#if defined(_WIN32)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
#endif
    if (!isatty(fileno(stdout)))
        goto non_windows;
#if defined(_WIN32)
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    if (osvi.dwMajorVersion < 10)
        return ColorFormat::EnableMode::Ansi;

    if (hOut == INVALID_HANDLE_VALUE)
        goto non_windows;

    if (!GetConsoleMode(hOut, &dwMode))
        goto non_windows;

    if (!SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        goto non_windows;

    result = ColorFormat::EnableMode::TrueColor;
#endif
non_windows:
    if (const char* colorterm = std::getenv("COLORTERM")) {
        std::cout << "colorterm: " << colorterm << std::endl;
        const std::string ct(colorterm);
        if (ct.find("truecolor") != std::string::npos ||
            ct.find("24bit") != std::string::npos)
            return ColorFormat::EnableMode::TrueColor;

        if (ct.find("8bit") != std::string::npos)
            return ColorFormat::EnableMode::Byte;
    }

    if (const char* term = std::getenv("TERM")) {
        std::cout << "term: " << term << std::endl;
        const std::string t(term);
        if (t.find("256color") != std::string::npos)
            return ColorFormat::EnableMode::Byte;
        if (t.find("color") != std::string::npos)
            return ColorFormat::EnableMode::Ansi;
        result = ColorFormat::EnableMode::TrueColor;
    }

    return result;
}

FileLoggerChannel::FileLoggerChannel(std::string&& path) :
    LoggerChannelBase(ColorFormat::EnableMode::Raw),
    output(std::ofstream(path)) {
}
