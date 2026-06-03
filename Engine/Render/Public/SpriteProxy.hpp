#pragma once

#include "GenericHandle.hpp"
#include "Primitives.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class SpriteRenderer;

    struct SpriteRenderItem{
        // for 8 * 8 sprite, [1/8, 1/8]
        Vec2 uvScale;
        // [cols, rows]
        Vec2 offset;

        RHITexture& texture;
    };

    class SpriteProxy final{
    public:
        using Handle = GenericHandle<SpriteRenderItem>;

    private:
        Handle handle = Handle::InvalidHandle();
        SpriteRenderer* renderer = nullptr;

    public:
        SpriteProxy() noexcept = default;
        ~SpriteProxy();
        SMOL_DECLARE_NON_COPYABLE(SpriteProxy)
        SpriteProxy(SpriteProxy&&) noexcept;
        SpriteProxy& operator=(SpriteProxy&&) noexcept;

        SpriteProxy(Handle handle, SpriteRenderer& renderer)
            : handle(handle), renderer(&renderer){}

        bool IsValid() const noexcept{
            auto isRendererValid = renderer != nullptr;
            auto isHandleValid = handle.IsValid();

            return isRendererValid && isHandleValid;
        }
        SpriteRenderItem& GetRenderItem() noexcept;
   };
}
