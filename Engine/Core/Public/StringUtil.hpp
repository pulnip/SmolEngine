#pragma once

#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>
#include "Primitives.hpp"

namespace Smol
{
    Str toUpper(StrView);

    struct StringHash{
        using is_transparent = void;

        usize operator()(StrView view) const{
            return std::hash<StrView>{}(view);
        }
    };
    template<typename T>
    using StringHashMap = std::unordered_map<Str, T, StringHash, std::equal_to<>>;

    Str readFileAsString(const std::filesystem::path&);
    std::vector<u8> readFileAsBinary(const std::filesystem::path&);
}
