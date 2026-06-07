#pragma once

#include "GenericHandle.hpp"
#include "Primitives.hpp"
#include "Resource.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class SpriteRenderer;

    struct SpriteRenderItem{
        using Handle = GenericHandle<SpriteResource>;

        Transform transform{
            .position = zeros(),
            .rotation = unitQuat(),
            .scale = ones()
        };
        // for 8 * 8 sprite, [1/8, 1/8]
        Vec2 uvScale{.x = 1.0f, .y = 1.0f};
        // [cols, rows]
        Vec2 offset{.x = 0, .y = 0};

        Handle handle;
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
