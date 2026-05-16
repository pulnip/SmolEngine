#pragma once

#include "DOM.hpp"
#include "TomlLoader.hpp"

namespace Smol
{
    struct RuntimeConfig{
        Str name = "AnonymousApp";
        Str version = "v0.0.1";
        Str identifier = "AnonymousIdentifier";

        struct WindowConfig{
            Str title = "DefaultWindow";
            u32 width = 800, height = 600;
            bool fullscreen = false;
            bool resizable = false;
            bool borderless = false;
            bool always_on_top = false;
        } window;
    };

    template<>
    struct TomlTraits<RuntimeConfig>{
        static RuntimeConfig from(const DOM::Value& root);
    };
}