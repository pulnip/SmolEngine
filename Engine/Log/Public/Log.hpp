#pragma once

#include <format>
#include "LogDefinitions.hpp"

namespace Smol
{
    namespace detail
    {
        void Log(LogMessage&&);
    }

    template<typename... Args>
    void Log(LogLevel level, CStr category,
        std::source_location location,
        std::format_string<Args...> fmt, Args&&... args
    ) noexcept{
        LogMessage msg{
            .level = level,
            .category = category,
            .text = std::format(fmt, std::forward<Args>(args)...),
            .location = location,
            .thread_id = std::this_thread::get_id(),
            .time_point = std::chrono::system_clock::now()
        };

        detail::Log(std::move(msg));
    }

    void SetLogLevel(LogLevel level);
}

#define LOG_IMPL(level, category, fmt, ...) \
    Smol::Log(level, category, std::source_location::current(), fmt, ##__VA_ARGS__)

#define LOG_TRACE(category, fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Trace, category, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(category, fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Debug, category, fmt, ##__VA_ARGS__)
#define LOG_INFO(category, fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Info, category, fmt, ##__VA_ARGS__)
#define LOG_WARN(category, fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Warn, category, fmt, ##__VA_ARGS__)
#define LOG_ERROR(category, fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Error, category, fmt, ##__VA_ARGS__)
#define LOG_FATAL(category, fmt, ...) \
    LOG_IMPL(Smol::LogLevel::Fatal, category, fmt, ##__VA_ARGS__)
