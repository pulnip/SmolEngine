#pragma once

#include "DOM.hpp"
#include "Primitives.hpp"
#include "RuntimeConfig.hpp"
#include "TomlLoader.hpp"

namespace Smol
{
    struct ProjectConfig{
        Str content_root;
    };

    struct BootConfig{
        Str statup_scene;
        Str default_input;
    };

    struct AppConfig{
        TomlMetadata metadata;

        RuntimeConfig runtime;
        ProjectConfig project;
        BootConfig boot;
    };

    template<>
    struct TomlTraits<AppConfig>{
        static AppConfig from(const TomlMetadata&, const DOM::Value& root);
    };
}
