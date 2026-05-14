#pragma once

#include <filesystem>
#include "DOM.hpp"

namespace Smol
{
    template<typename T>
    struct TomlTraits;

    DOM::Value parseTomlString(StrView);
    DOM::Value parseTomlFile(const std::filesystem::path&);

    template<typename T>
    T loadToml(StrView str){
        auto tbl = parseTomlString(str);

        return TomlTraits<T>::from(tbl);
    }

    template<typename T>
    T loadTomlFile(const std::filesystem::path& path){
        auto tbl = parseTomlFile(path);

        return TomlTraits<T>::from(tbl);
    }
}