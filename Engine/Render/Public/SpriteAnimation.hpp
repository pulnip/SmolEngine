#pragma once

#include "Primitives.hpp"
#include "RHIFWD.hpp"

namespace Smol
{
    struct SpriteRenderItem{
        // for 8 * 8 sprite, [1/8, 1/8]
        Vec2 uvScale;
        // [cols, rows]
        Vec2 offset;

        RHITexture& texture;
    };
}
