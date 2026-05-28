#include "AppConfig.hpp"
#include "OS.hpp"

namespace Smol
{
    AppConfig TomlTraits<AppConfig>::from(
        const TomlMetadata& metadata,
        const DOM::Value& root
    ){
        // Project Config
        auto content_root = root.get<Str>("project.content_root")
            .value_or("Content/");

        // Boot Config
        auto startup_scene = root.get<Str>("project.startup_scene")
            .value_or(Str{});
        auto default_input = root.get<Str>("project.default_input")
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
