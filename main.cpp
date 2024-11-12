#include <iostream>
#include <logger/format.h>
#include <logger/channel.h>
// #include <logger/color.h>

int main()
{
    std::cout.tie(nullptr);
    std::ios::sync_with_stdio(false);

    ColorFormat color;
    color.set_8bit(10, color_literals::Foreground);
    FontFormat font;
    font.set_bold(true);
    auto ansi = "hello world"_ansi;
    std::cout << color.apply(ansi) << std::endl;
    std::cout << font.apply(ansi) << std::endl;
}
