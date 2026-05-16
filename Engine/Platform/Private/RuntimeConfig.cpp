#include "RuntimeConfig.hpp"

namespace Smol
{
    RuntimeConfig TomlTraits<RuntimeConfig>::from(
        const DOM::Value& root
    ){
        RuntimeConfig config;

        config.name = root.get<Str>("metadata.name")
            .value_or("AnonymousApp");
        config.version = root.get<Str>("metadata.version")
            .value_or("v0.0.1");
        config.identifier = root.get<Str>("metadata.identifier")
            .value_or("AnonymousIdentifier");

        auto& window = config.window;
        window.title = root.get<Str>("window.title")
            .value_or("DefaultWindow");
        window.width = root.get<u32>("window.width")
            .value_or(800);
        window.height = root.get<u32>("window.height")
            .value_or(600);
        window.fullscreen = root.get<bool>("window.fullscreen")
            .value_or(false);
        window.resizable = root.get<bool>("window.resizable")
            .value_or(false);
        window.borderless = root.get<bool>("window.borderless")
            .value_or(false);
        window.always_on_top = root.get<bool>("window.always_on_top")
            .value_or(false);

        return config;
    }
}