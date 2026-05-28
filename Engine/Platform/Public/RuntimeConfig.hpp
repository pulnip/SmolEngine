#pragma once

#include "DOM.hpp"
#include "Primitives.hpp"
#include "TomlLoader.hpp"

namespace Smol
{
    struct WindowConfig{
        Str title = "DefaultWindow";
        u32 width = 800, height = 600;

        bool fullscreen = false;
        bool resizable = false;
        bool borderless = false;
        bool always_on_top = false;
    };

    struct RuntimeConfig{
        Str name = "AnonymousApp";
        Str version = "v0.0.1";
        Str identifier = "AnonymousIdentifier";

        WindowConfig window;
    };

    template<>
    struct TomlTraits<RuntimeConfig>{
        static RuntimeConfig from(const DOM::Value& root);
    };
}
