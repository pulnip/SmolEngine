#include <algorithm>
#include "Actor.hpp"
#include "Assert.hpp"
#include "ResourceManager.hpp"
#include "RHITexture.hpp"
#include "SpriteAnimComponent.hpp"
#include "SpriteRenderer.hpp"
#include "World.hpp"

namespace Smol
{
    SMOL_COMPONENT(SpriteAnimComponent)
        .SetProperty("sprite_scale", &SpriteAnimComponent::spriteScale)
    SMOL_COMPONENT_END(SpriteAnimComponent)

    void SpriteAnimComponent::OnAttach(StrView key){
        SMOL_ASSERT(!handle.IsValid());

        auto world = owner->GetWorld();
        auto spriteManager = world->GetSpriteManager();
        auto spriteRenderer = world->GetSpriteRenderer();

        handle = spriteManager->Find(key);
        SMOL_ASSERT(handle.IsValid());

        proxy = spriteRenderer->BindRenderItem(handle);
        SMOL_ASSERT(proxy.IsValid());

        // Initialize RenderItem
        synced = false;
        syncToRenderer();

        SetAnimation("idle");
    }

    void SpriteAnimComponent::Update(f32 dt){
        elapsedTime += std::min(dt, framePerSeconds);

        if(elapsedTime > framePerSeconds){
            elapsedTime -= framePerSeconds;
            nextFrame();
        }

        syncToRenderer();
    }

    void SpriteAnimComponent::SetAnimation(StrView anim){
        auto world = owner->GetWorld();
        auto spriteManager = world->GetSpriteManager();
        const auto& animations = spriteManager->GetRef(handle).animations;

        if(auto it = animations.find(anim); it != animations.end()){
            auto& animation = it->second;
            startRow = animation.startRow;
            startCol = animation.startCol;
            proxy.GetRenderItem().offset = {
                static_cast<float>(startCol),
                static_cast<float>(startRow)
            };
            frameCount = animation.frameCount;
            framePerSeconds = animation.frameDurationMs / 1000.0f;
        }
    }

    void SpriteAnimComponent::syncToRenderer(){
        auto& item = proxy.GetRenderItem();
        item.transform = owner->GetWorldTransform();
        item.spriteScale = toVec3(spriteScale, 1.0f);

        if(synced) return;

        item.offset = Vec2{
            static_cast<f32>(startCol + iframe),
            static_cast<f32>(startRow)
        };

        synced = true;
    }

    void SpriteAnimComponent::nextFrame() noexcept{
        iframe = (iframe + 1) % frameCount;
        synced = false;
    }
}
