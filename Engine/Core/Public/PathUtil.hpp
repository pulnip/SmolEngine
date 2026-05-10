#pragma once

#include <cstddef>
#include <filesystem>
#include <format>
#include <string>

namespace Smol
{
    std::filesystem::path get_executable_dir();
    std::filesystem::path get_absolute_path(const std::filesystem::path&);

    std::filesystem::path to_path(const char* utf8Str);
    std::filesystem::path to_path(const char* utf8Str, size_t len);
    std::string to_utf8String(const std::filesystem::path&);
}

template<>
struct std::formatter<std::filesystem::path>: std::formatter<std::string>{
    inline auto format(const std::filesystem::path& p, auto& ctx) const{
        auto u8str = Smol::to_utf8String(p);
        return std::formatter<std::string>::format(u8str, ctx);
    }
};
