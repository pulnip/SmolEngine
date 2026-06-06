#include <stdexcept>
#include <algorithm>
#include "Assert.hpp"
#include "LogLocal.hpp"
#include "RHITexture.hpp"
#include "SpawnContext.hpp"
#include "SpriteComponent.hpp"
#include "SpriteRenderer.hpp"

namespace Smol
{
    static Transform trs;

    SMOL_COMPONENT(SpriteComponent)
    SMOL_COMPONENT_END(SpriteComponent)

    SpriteComponent::~SpriteComponent() = default;

    SpriteComponent::SpriteComponent(
        RHITextureRAII&& sprite,
        Transform& transform,
        SpriteRenderer& renderer
    )
        : proxy(renderer.BindRenderItem(*sprite))
        , sprite(std::move(sprite))
        , transform(transform)
    {
        SMOL_ASSERT(proxy.IsValid());

        auto& item = proxy.GetRenderItem();
        item.transform = transform;

        transform.scale = Vec3(4, 4, 1);
        // TODO. change later
        item.uvScale = {1.0f/16, 1.0f/16};
        item.offset = {0, 0};
    }

    SpriteComponent::SpriteComponent(const SpawnContext& ctx)
        // : proxy(ctx.spriteRenderer->BindRenderItem(*sprite))
        // , sprite(std::move(sprite))
        : transform(trs)
    {
        throw std::runtime_error("Unimplemented");
    }

    void SpriteComponent::Update(f32 dt){
        elapsedTime += std::min(dt, framePerSeconds);

        auto& item = proxy.GetRenderItem();
        item.transform = transform;

        if(elapsedTime > framePerSeconds){
            elapsedTime -= framePerSeconds;
            nextFrame();
        }

        syncToRenderer();
    }

    void SpriteComponent::syncToRenderer(){
        if(synced) return;

        auto& item = proxy.GetRenderItem();
        item.offset = Vec2{static_cast<f32>(iframe), 0.0f};

        synced = true;
    }

    void SpriteComponent::nextFrame() noexcept{
        iframe = (iframe + 1) % frameCount;
        synced = false;
    }
}
