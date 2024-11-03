#ifndef CHANNEL_H
#define CHANNEL_H

#include <fstream>
#include <string>
#include <logger/color.h>

class LoggerChannelBase {
public:
    LoggerChannelBase() = delete;

    virtual ~LoggerChannelBase() = default;
protected:
    ColorFormatPipe pipe;

    explicit LoggerChannelBase(ColorFormat::EnableMode);
public:
    ColorFormat& add_format();
    ColorFormat& new_format();
    virtual void send(std::string&&) = 0;
};

class StdLoggerChannel final : public LoggerChannelBase {
private:
    std::ostream& output;
public:
    explicit StdLoggerChannel(std::ostream&&);

    static ColorFormat::EnableMode enable_mode();
    void send(std::string&&) override;
};

class FileLoggerChannel final : public LoggerChannelBase {
private:
    std::ofstream output;
public:
    explicit FileLoggerChannel(std::string&&);
    void send(std::string&&) override;
};

#endif