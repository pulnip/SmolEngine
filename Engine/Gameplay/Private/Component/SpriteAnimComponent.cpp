#include <algorithm>
#include "Actor.hpp"
#include "Assert.hpp"
#include "ResourceManager.hpp"
#include "RHITexture.hpp"
#include "SpawnContext.hpp"
#include "SpriteAnimComponent.hpp"
#include "SpriteConfig.hpp"
#include "SpriteRenderer.hpp"

namespace Smol
{
    SMOL_COMPONENT(SpriteAnimComponent)
    SMOL_COMPONENT_END(SpriteAnimComponent)

    SpriteAnimComponent::~SpriteAnimComponent() = default;

    SpriteAnimComponent::SpriteAnimComponent(const SpawnContext& ctx)
        : owner(*ctx.owner)
    {
        SMOL_ASSERT(ctx.owner != nullptr);

        const auto request = createSpriteRequest(
            ctx.dom,
            ctx.contentRoot
        );
        handle = ctx.spriteManager->Load(request);
        proxy = ctx.spriteRenderer->BindRenderItem(handle);
        SMOL_ASSERT(proxy.IsValid());

        auto& item = proxy.GetRenderItem();
        item.transform = owner.GetTransform();

        auto spriteScale = ctx.dom.get<Vec2>("sprite_scale")
            .value_or(Vec2(1, 1));
        item.spriteScale = toVec3(spriteScale, 1.0f);

        const auto& size = request.sheetSize;
        item.uvScale = {1.0f/size.x, 1.0f/size.y};

        const auto& animations = request.animations;
        // TODO. change later
        if(auto it = animations.find("walk"); it != animations.end()){
            auto& animation = it->second;
            item.offset = {
                static_cast<float>(animation.startCol),
                static_cast<float>(animation.startRow)
            };
            frameCount = animation.frameCount;
            framePerSeconds = animation.frameDurationMs / 1000.0f;
        }
    }

    void SpriteAnimComponent::Update(f32 dt){
        elapsedTime += std::min(dt, framePerSeconds);

        auto& item = proxy.GetRenderItem();
        item.transform = owner.GetTransform();

        if(elapsedTime > framePerSeconds){
            elapsedTime -= framePerSeconds;
            nextFrame();
        }

        syncToRenderer();
    }

    void SpriteAnimComponent::syncToRenderer(){
        if(synced) return;

        auto& item = proxy.GetRenderItem();
        item.offset = Vec2{static_cast<f32>(iframe), 0.0f};

        synced = true;
    }

    void SpriteAnimComponent::nextFrame() noexcept{
        iframe = (iframe + 1) % frameCount;
        synced = false;
    }
}
