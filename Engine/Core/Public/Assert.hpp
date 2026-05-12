#pragma once

#include <format>
#include <source_location>
#include <string_view>
#include <cassert>

namespace Smol
{
    namespace detail{
        [[noreturn]] void assertFail(
            const char* expr,
            std::source_location,
            std::string_view msg = {}
        ) noexcept;
    }
}

#if defined(_DEBUG) || !defined(NDEBUG)
    #define SMOL_ASSERT(expr, ...) \
        do{ \
            if(!(expr)) [[unlikely]]{ \
                Smol::detail::assertFail( \
                    #expr, \
                    std::source_location::current() \
                    __VA_OPT__(, std::format(__VA_ARGS__)) \
                ); \
            } \
        } while(false)
#else
    #define SMOL_ASSERT(expr, ...) ((void)0)
#endif
