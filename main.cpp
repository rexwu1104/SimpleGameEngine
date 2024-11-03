#include <iostream>
// #include <utils/logger/logger.h>
#include <logger/channel.h>
#include <logger/color.h>

int main()
{
    std::cout.tie(nullptr);
    std::ios::sync_with_stdio(false);

    StdLoggerChannel channel(std::ostream(std::cout.rdbuf()));
    channel.new_format().set_24bit(0, 255, 255, ColorFormat::AnsiMode::Foreground);
    channel.add_format().set_24bit(255, 149, 202, ColorFormat::AnsiMode::Background);
    channel.send("Hello world");

    FileLoggerChannel channel2("./test.log");
    channel2.send("Hello world");
}
