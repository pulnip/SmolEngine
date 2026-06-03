#pragma once

#include "Primitives.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"
#include "SlotMap.hpp"
#include "SpriteProxy.hpp"

namespace Smol
{
    class SpriteRenderer final{
    public:
        using Handle = SpriteProxy::Handle;

    private:
        SlotMap<SpriteRenderItem> renderItems;

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

        [[nodiscard]]
        SpriteProxy BindRenderItem(RHITexture&);

        void Draw(RHICommandList&);

    private:
        friend class SpriteProxy;

        SpriteRenderItem& GetRenderItem(Handle);
        void UnbindRenderItem(Handle);
    };
}
