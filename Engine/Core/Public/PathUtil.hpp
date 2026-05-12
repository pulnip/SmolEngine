#pragma once

#include <cstddef>
#include <filesystem>
#include <format>
#include <string>

namespace Smol
{
    std::filesystem::path getExecutableDir();
    std::filesystem::path getAbsolutePath(const std::filesystem::path&);

    std::filesystem::path toPath(const char* utf8Str);
    std::filesystem::path toPath(const char* utf8Str, size_t len);
    std::string toUTF8String(const std::filesystem::path&);
}

template<>
struct std::formatter<std::filesystem::path>: std::formatter<std::string>{
    inline auto format(const std::filesystem::path& p, auto& ctx) const{
        auto u8str = Smol::toUTF8String(p);
        return std::formatter<std::string>::format(u8str, ctx);
    }
};
