#include "Actor.hpp"
#include "Assert.hpp"
#include "ResourceManager.hpp"
#include "RHITexture.hpp"
#include "SpawnContext.hpp"
#include "SpriteComponent.hpp"
#include "SpriteRenderer.hpp"

namespace{
    auto createSpriteRequest(const Smol::DOM::Value& dom){
        using namespace Smol;

        // use sprite meta file
        auto path = dom.get<Str>("image_path")
            .value_or("");

        return SpriteRequest{
            .path = path,
            .frameCount = 0,
            .framePerSeconds = 0.0f
        };
    }
}

namespace Smol
{
    SMOL_COMPONENT(SpriteComponent)
    SMOL_COMPONENT_END(SpriteComponent)

    SpriteComponent::~SpriteComponent() = default;

    SpriteComponent::SpriteComponent(const SpawnContext& ctx)
        : handle(ctx.spriteManager->Load(createSpriteRequest(ctx.dom)))
        // TODO.
        , proxy(ctx.spriteRenderer->BindRenderItem(handle))
        , transform(ctx.owner->GetTransform())
    {
        SMOL_ASSERT(proxy.IsValid());

        auto& item = proxy.GetRenderItem();
        item.transform = transform;

        auto spriteScale = ctx.dom.get<Vec2>("sprite_scale")
            .value_or(Vec2(1, 1));
        item.spriteScale = toVec3(spriteScale, 1.0f);

        item.uvScale = {1.0f/16, 1.0f/16};
        item.offset = {0, 0};
    }

    void SpriteComponent::Update(f32){
        auto& item = proxy.GetRenderItem();
        item.transform = transform;
    }
}
