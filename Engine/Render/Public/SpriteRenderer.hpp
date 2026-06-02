#pragma once

#include <span>
#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    struct SpriteRenderItem;

    class SpriteRenderer final{
    private:
        RHIGraphicsPipelineStateRAII pipeline;

    public:
        SpriteRenderer(const RHIDevice&);
        ~SpriteRenderer();
        SMOL_DECLARE_MOVE_ONLY(SpriteRenderer)

        void Draw(const SpriteRenderItem&);
    };
}
