#pragma once

#include <span>
#include "Primitives.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    struct SpriteRenderItem;

    class SpriteRenderer final{
    private:
        RHIGraphicsPipelineStateRAII pipeline;
        RHISamplerRAII sampler;

        // slot from Shader Reflection
        struct VertexShaderSlot{
            static constexpr CStr SpriteConstantsSlot = "spriteConstants";
            u32 spriteConstants = 0;
        } vs;
        struct FragmentShaderSlot{
            static constexpr CStr texSlot = "tex";
            u32 tex = 0;
            static constexpr CStr sampSlot = "samp";
            u32 samp = 0;
        } fs;

    public:
        SpriteRenderer(RHIDevice&);
        ~SpriteRenderer();
        SMOL_DECLARE_MOVE_ONLY(SpriteRenderer)

        void Draw(RHICommandList&, std::span<const SpriteRenderItem>);
    };
}
