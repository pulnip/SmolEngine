#include "Assert.hpp"
#include "RHITexture.hpp"
#include "SpriteComponent.hpp"
#include "SpriteRenderer.hpp"

namespace Smol
{
    SpriteComponent::SpriteComponent() = default;
    SpriteComponent::~SpriteComponent() = default;

    SpriteComponent::SpriteComponent(
        RHITextureRAII&& sprite,
        SpriteRenderer& renderer)
        : sprite(std::move(sprite))
        , proxy(renderer.BindRenderItem(*sprite))
    {
        SMOL_ASSERT(proxy.IsValid());

        auto& item = proxy.GetRenderItem();
        // TODO.
        frameCount = 8;
        item.uvScale = {1.0f/16, 1.0f/16};
    }

    void SpriteComponent::SyncToRenderer(){
        if(synced) return;

        auto& item = proxy.GetRenderItem();
        item.offset = Vec2{static_cast<f32>(iframe), 0.0f};

        synced = true;
    }

    void SpriteComponent::NextFrame() noexcept{
        iframe = (iframe + 1) % frameCount;
        synced = false;
    }
}
