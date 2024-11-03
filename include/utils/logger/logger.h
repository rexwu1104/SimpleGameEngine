#ifndef LOGGER_H
#define LOGGER_H

#include <map>
#include <string>

#include <logger/channel.h>

template<typename Derived>
class LoggerBase {
protected:
    using DeriveClass = Derived;

    std::map<std::string, LoggerChannelBase> channel_table;
};

#endif
