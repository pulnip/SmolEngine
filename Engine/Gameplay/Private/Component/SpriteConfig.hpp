#pragma once

#include <filesystem>
#include "Resource.hpp"

namespace Smol
{
    namespace DOM
    {
        class Value;
    }

    SpriteRequest createSpriteRequest(const DOM::Value& dom, const std::filesystem::path& contentRoot);
}
