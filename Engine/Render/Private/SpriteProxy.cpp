#include "SpriteProxy.hpp"
#include "SpriteRenderer.hpp"

namespace Smol
{
    SpriteProxy::SpriteProxy(SpriteProxy&& other) noexcept
        : handle(other.handle)
        , renderer(other.renderer)
    {
        other.handle = Handle::InvalidHandle();
        other.renderer = nullptr;
    }
    SpriteProxy& SpriteProxy::operator=(SpriteProxy&& other) noexcept{
        handle = other.handle;
        renderer = other.renderer;

        other.handle = Handle::InvalidHandle();
        other.renderer = nullptr;

        return *this;
    }

    SpriteProxy::~SpriteProxy(){
        if(IsValid()){
            renderer->UnbindRenderItem(handle);

            handle = Handle::InvalidHandle();
            renderer = nullptr;
        }
    }

    SpriteRenderItem& SpriteProxy::GetRenderItem() noexcept{
        SMOL_ASSERT(renderer != nullptr);
        return renderer->GetRenderItem(handle);
    }
}
