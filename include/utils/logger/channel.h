#ifndef CHANNEL_H
#define CHANNEL_H

#include <fstream>
#include <string>

class LoggerChannelBase {
public:
    LoggerChannelBase() = delete;

    virtual ~LoggerChannelBase() = default;
protected:
public:
    virtual void send(std::string&&) = 0;
};

class StdLoggerChannel final : public LoggerChannelBase {
private:
    std::ostream output;
public:
    explicit StdLoggerChannel(const std::ostream&);

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