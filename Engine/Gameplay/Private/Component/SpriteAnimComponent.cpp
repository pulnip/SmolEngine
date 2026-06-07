#include <algorithm>
#include "Actor.hpp"
#include "Assert.hpp"
#include "ResourceManager.hpp"
#include "RHITexture.hpp"
#include "SpawnContext.hpp"
#include "SpriteAnimComponent.hpp"
#include "SpriteRenderer.hpp"

namespace{
    auto createSpriteRequest(const Smol::DOM::Value& dom){
        using namespace Smol;

        // use sprite meta file
        auto path = dom.get<Str>("image_path")
            .value_or("");
        auto frameCount = dom.get<u32>("frame_count")
            .value_or(1);
        auto framePerSeconds = dom.get<f32>("frame_per_seconds")
            .value_or(0.16);

        return SpriteRequest{
            .path = path,
            .frameCount = frameCount,
            .framePerSeconds = framePerSeconds
        };
    }
}

namespace Smol
{
    SMOL_COMPONENT(SpriteAnimComponent)
    SMOL_COMPONENT_END(SpriteAnimComponent)

    SpriteAnimComponent::~SpriteAnimComponent() = default;

    SpriteAnimComponent::SpriteAnimComponent(const SpawnContext& ctx)
        : handle(ctx.spriteManager->Load(createSpriteRequest(ctx.dom)))
        // TODO.
        , frameCount(ctx.dom.get<u32>("frame_count").value_or(8))
        , framePerSeconds(ctx.dom.get<f32>("frame_per_seconds").value_or(0.16f))
        , proxy(ctx.spriteRenderer->BindRenderItem(handle))
        , transform(ctx.owner->GetTransform())
    {
        SMOL_ASSERT(proxy.IsValid());

        auto& item = proxy.GetRenderItem();
        item.transform = transform;

        auto spriteScale = ctx.dom.get<Vec2>("sprite_scale")
            .value_or(Vec2(1, 1));
        item.spriteScale = toVec3(spriteScale, 1.0f);

        // TODO. change later
        item.uvScale = {1.0f/16, 1.0f/16};
        item.offset = {0, 0};
    }

    void SpriteAnimComponent::Update(f32 dt){
        elapsedTime += std::min(dt, framePerSeconds);

        auto& item = proxy.GetRenderItem();
        item.transform = transform;

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
