#include "Assert.hpp"
#include "AppConfig.hpp"
#include "OS.hpp"

namespace Smol
{
    AppConfig TomlTraits<AppConfig>::from(
        const DOM::Value& root,
        const TomlMetadata& metadata
    ){
        SMOL_ASSERT(metadata.type == "app");

        // Project Config
        auto content_root = root.get<Str>("project.content_root")
            .value_or("Content/");

        // Boot Config
        auto resource_manifest = root.get<Str>("boot.resource_manifest")
            .value_or(Str{});
        auto prefab_manifest = root.get<Str>("boot.prefab_manifest")
            .value_or(Str{});
        auto default_input = root.get<Str>("boot.default_input")
            .value_or(Str{});
        auto startup_scene = root.get<Str>("boot.startup_scene")
            .value_or(Str{});

        return AppConfig{
            .metadata = metadata,
            .runtime = TomlTraits<RuntimeConfig>::from(root),
            .project = ProjectConfig{
                .content_root = content_root
            },
            .boot = BootConfig{
                .resource_manifest = resource_manifest,
                .prefab_manifest = prefab_manifest,
                .default_input = default_input,
                .statup_scene = startup_scene
            }
        };
    }

}
