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

    struct TomlMetadata{
        u32 version;
        Str name;
    };
    template<>
    struct TomlTraits<TomlMetadata>{
        static TomlMetadata from(const DOM::Value& root);
    };

    template<typename T>
    T loadTomlFile(const std::filesystem::path& path){
        auto tbl = parseTomlFile(path);
        auto metadata = TomlTraits<TomlMetadata>::from(tbl);

        return TomlTraits<T>::from(metadata, tbl);
    }
}
