#pragma once

#include <filesystem>
#include <string_view>
#include "DOM.hpp"

namespace Smol
{
    template<typename T>
    struct TomlTraits;

    DOM::Value parseTomlString(std::string_view);
    DOM::Value parseTomlFile(const std::filesystem::path&);

    template<typename T>
    T loadToml(std::string_view str){
        auto tbl = parseTomlString(str);

        return TomlTraits<T>::from(tbl);
    }

    // template<typename T>
    // T loadToml(const std::filesystem::path& path){
    //     auto tbl = parseTomlFile(path);

    //     return TomlTraits<T>::from(tbl);
    // }
}