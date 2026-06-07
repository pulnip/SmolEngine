#pragma once

#include "Primitives.hpp"
#include "Resource.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"
#include "SlotMap.hpp"
#include "SpriteProxy.hpp"

namespace Smol
{
    template<Resource T>
    class ResourceManager;

    class SpriteRenderer final{
    public:
        using ResourceHandle = SpriteRenderItem::Handle;
        using Handle = SpriteProxy::Handle;

    private:
        ResourceManager<SpriteResource>& spriteManager;
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
        SpriteRenderer(RHIDevice&, ResourceManager<SpriteResource>&);
        ~SpriteRenderer();
        SMOL_DECLARE_PINNED(SpriteRenderer)

        [[nodiscard]]
        SpriteProxy BindRenderItem(ResourceHandle);

        void Draw(RHICommandList&);

    private:
        friend class SpriteProxy;

        SpriteRenderItem& GetRenderItem(Handle);
        void UnbindRenderItem(Handle);
    };
}
