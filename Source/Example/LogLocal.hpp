#pragma once

#include "Log.hpp"

#undef LOG_TRACE
#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#undef LOG_FATAL

#define LOG_TRACE(fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Trace, "Example", fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Debug, "Example", fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Info, "Example", fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Warn, "Example", fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Error, "Example", fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Fatal, "Example", fmt, ##__VA_ARGS__)
