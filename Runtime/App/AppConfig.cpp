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
        auto startup_scene = root.get<Str>("boot.startup_scene")
            .value_or(Str{});
        auto default_input = root.get<Str>("boot.default_input")
            .value_or(Str{});

        return AppConfig{
            .metadata = metadata,
            .runtime = TomlTraits<RuntimeConfig>::from(root),
            .project = ProjectConfig{
                .content_root = content_root
            },
            .boot = BootConfig{
                .statup_scene = startup_scene,
                .default_input = default_input
            }
        };
    }

}
