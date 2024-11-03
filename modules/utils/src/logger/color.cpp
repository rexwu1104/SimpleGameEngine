#include <iostream>
#include <numeric>
#include <sstream>
#include <logger/color.h>

ColorFormat::ColorFormat(EnableMode enabled) :
    color_param({ AnsiColor::White }),
    mode(AnsiMode::Foreground),
    more(MoreMode::None),
    enabled(enabled) {
}

void ColorFormat::set_basic(const AnsiColor color, const AnsiMode mode) {
    if (enabled < EnableMode::Ansi || color == AnsiColor::More)
        return;

    this->color_param = { .basic = color };
    this->mode = mode;
    this->more = MoreMode::None;
}

void ColorFormat::set_8bit(AnsiColor color, AnsiMode mode) {
    if (enabled < EnableMode::Byte || color == AnsiColor::More)
        return;

    const int mode_i = static_cast<unsigned char>(mode);
    this->color_param = { .bits8 = static_cast<unsigned char>(color) + (mode_i > 5 ? 8 : 0) };
    this->mode = mode_i % 2 ? AnsiMode::Foreground : AnsiMode::Background;
    this->more = MoreMode::Bits8;
}

void ColorFormat::set_8bit(unsigned char r, unsigned char g, unsigned char b, AnsiMode mode) {
    if (enabled < EnableMode::Byte || r > 5 || g > 5 || b > 5)
        return;

    const int mode_i = static_cast<unsigned char>(mode);
    this->color_param = { .bits8 = 16 + 36 * r + 6 * g + b };
    this->mode = mode_i % 2 ? AnsiMode::Foreground : AnsiMode::Background;
    this->more = MoreMode::Bits8;
}

void ColorFormat::set_8bit(unsigned char gray, AnsiMode mode) {
    if (enabled < EnableMode::Byte || gray >= 24)
        return;

    const int mode_i = static_cast<unsigned char>(mode);
    this->color_param = { .bits8 = 232 + gray };
    this->mode = mode_i % 2 ? AnsiMode::Foreground : AnsiMode::Background;
    this->more = MoreMode::Bits8;
}

void ColorFormat::set_24bit(unsigned char r, unsigned char g, unsigned char b, AnsiMode mode) {
    if (enabled < EnableMode::TrueColor)
        return;

    const int mode_i = static_cast<unsigned char>(mode);
    this->color_param = { .bits24 = { r, g, b } };
    this->mode = mode_i % 2 ? AnsiMode::Foreground : AnsiMode::Background;
    this->more = MoreMode::Bits24;
}

std::string ColorFormat::prefix() {
    std::stringstream prefix;
    prefix << "\033[" << static_cast<int>(mode);
    switch (more) {
    case MoreMode::None:
        prefix << static_cast<int>(color_param.basic);
        goto end;
    default:
        prefix << 8 << ';' << static_cast<int>(more);
    }

    prefix << ";";
    switch (more) {
    default:
    case MoreMode::Bits8:
        prefix << (int)color_param.bits8;
        goto end;
    case MoreMode::Bits24:
        prefix << (int)color_param.bits24.r << ';'
               << (int)color_param.bits24.g << ';'
               << (int)color_param.bits24.b;
    }
end:
    prefix << "m";
    return prefix.str();
}

std::string ColorFormat::apply(const std::string& str, const bool reset) {
    return prefix() + str + (reset ? "\033[0m" : "");
}

ColorFormatPipe::ColorFormatPipe(ColorFormat::EnableMode mode) :
    default_mode(mode) {
}

std::string ColorFormatPipe::apply(std::string &str) {
    for (auto it = formats.begin(); it != formats.end(); ++it)
        if (it == formats.end() - 1)
            str = it->apply(str);
        else
            str = it->apply(str, false);
    return str;
}

ColorFormat& ColorFormatPipe::add() {
    formats.emplace_back(default_mode);
    return formats[formats.size() - 1];
}

void ColorFormatPipe::reset() {
    formats.clear();
}


