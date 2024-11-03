#ifndef COLOR_H
#define COLOR_H

#include <vector>

class ColorFormat {
public:
    enum class AnsiColor {
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        More
    };

    enum class AnsiMode {
        Foreground = 3,
        Background,
        BrightForeground = 9,
        BrightBackground
    };

    enum class MoreMode {
        None,
        Bits8 = 5,
        Bits24 = 2,
    };

    enum class EnableMode {
        Raw,
        Ansi,
        Byte,
        TrueColor
    };
private:
    union {
        AnsiColor basic;
        unsigned char bits8;
        struct {
            unsigned char r, g, b;
        } bits24;
    } color_param;

    AnsiMode mode;
    MoreMode more;
    const EnableMode enabled;

    std::string prefix();
public:
    explicit ColorFormat(EnableMode);

    void set_basic(AnsiColor color, AnsiMode mode);
    void set_8bit(AnsiColor color, AnsiMode mode);
    void set_8bit(unsigned char r, unsigned char g, unsigned char b, AnsiMode mode);
    void set_8bit(unsigned char gray, AnsiMode mode);
    void set_24bit(unsigned char r, unsigned char g, unsigned char b, AnsiMode mode);
    std::string apply(const std::string& str, bool reset = true);
};

class ColorFormatPipe {
private:
    std::vector<ColorFormat> formats;
    ColorFormat::EnableMode default_mode;
public:
    explicit ColorFormatPipe(ColorFormat::EnableMode);

    std::string apply(std::string& str);
    ColorFormat& add();
    void reset();
};

#endif
