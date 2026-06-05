#pragma once

#include <filesystem>
#include "DOM.hpp"
#include "Primitives.hpp"
#include "RuntimeConfig.hpp"
#include "TomlLoader.hpp"

namespace Smol
{
    struct ProjectConfig{
        std::filesystem::path content_root;
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

        auto startupScenePath() const{
            return project.content_root / boot.statup_scene;
        }
        auto defaultInputPath() const{
            return project.content_root / boot.default_input;
        }
    };

    template<>
    struct TomlTraits<AppConfig>{
        static AppConfig from(const DOM::Value&, const TomlMetadata&);
    };
}
