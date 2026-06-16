#include "Actor.hpp"
#include "Assert.hpp"
#include "ResourceManager.hpp"
#include "RHITexture.hpp"
#include "SpriteComponent.hpp"
#include "SpriteRenderer.hpp"
#include "World.hpp"

namespace Smol
{
    SMOL_COMPONENT(SpriteComponent)
        .SetProperty("sprite_scale", &SpriteComponent::spriteScale)
    SMOL_COMPONENT_END(SpriteComponent)

    void SpriteComponent::OnAttach(StrView key){
        SMOL_ASSERT(!handle.IsValid());

        auto world = owner->GetWorld();
        auto spriteManager = world->GetSpriteManager();
        auto spriteRenderer = world->GetSpriteRenderer();

        handle = spriteManager->Find(key);
        SMOL_ASSERT(handle.IsValid());

        proxy = spriteRenderer->BindRenderItem(handle);
        SMOL_ASSERT(proxy.IsValid());

        // Initialize RenderItem
        syncToRenderer();
    }

    void SpriteComponent::Update(f32){
        syncToRenderer();
    }

    void SpriteComponent::syncToRenderer(){
        auto& item = proxy.GetRenderItem();
        item.transform = owner->GetWorldTransform();
        item.spriteScale = toVec3(spriteScale, 1.0f);
    }
}
