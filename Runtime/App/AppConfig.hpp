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
        Str resource_manifest;
        Str prefab_manifest;
        Str default_input;
        Str statup_scene;
    };

    struct AppConfig{
        TomlMetadata metadata;

        RuntimeConfig runtime;
        ProjectConfig project;
        BootConfig boot;

        auto resourceManifest() const{
            return project.content_root / boot.resource_manifest;
        }
        auto prefabManifest() const{
            return project.content_root / boot.prefab_manifest;
        }
        auto defaultInputPath() const{
            return project.content_root / boot.default_input;
        }
        auto startupScenePath() const{
            return project.content_root / boot.statup_scene;
        }
    };

    template<>
    struct TomlTraits<AppConfig>{
        static AppConfig from(const DOM::Value&, const TomlMetadata&);
    };
}
