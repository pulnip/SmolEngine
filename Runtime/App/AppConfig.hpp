#pragma once

#include "DOM.hpp"
#include "RuntimeConfig.hpp"
#include "TomlLoader.hpp"

namespace Smol
{
    struct AppConfig{
        RuntimeConfig runtime;
    };

    template<>
    struct TomlTraits<AppConfig>{
        static AppConfig from(const DOM::Value& root);
    };
}