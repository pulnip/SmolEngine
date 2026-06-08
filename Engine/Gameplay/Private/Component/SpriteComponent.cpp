#include "Actor.hpp"
#include "Assert.hpp"
#include "ResourceManager.hpp"
#include "RHITexture.hpp"
#include "SpawnContext.hpp"
#include "SpriteComponent.hpp"
#include "SpriteConfig.hpp"
#include "SpriteRenderer.hpp"

namespace Smol
{
    SMOL_COMPONENT(SpriteComponent)
    SMOL_COMPONENT_END(SpriteComponent)

    SpriteComponent::~SpriteComponent() = default;

    SpriteComponent::SpriteComponent(const SpawnContext& ctx)
        : TypedComponent(ctx.owner)
    {
        SMOL_ASSERT(owner != nullptr);

        const auto request = createSpriteRequest(
            ctx.dom,
            ctx.contentRoot
        );
        handle = ctx.spriteManager->Load(request);
        proxy = ctx.spriteRenderer->BindRenderItem(handle);
        SMOL_ASSERT(proxy.IsValid());

        auto& item = proxy.GetRenderItem();
        item.transform = owner->GetTransform();

        auto spriteScale = ctx.dom.get<Vec2>("sprite_scale")
            .value_or(Vec2(1, 1));
        item.spriteScale = toVec3(spriteScale, 1.0f);

        const auto& size = request.sheetSize;
        item.uvScale = {1.0f/size.x, 1.0f/size.y};
        item.offset = {0, 0};
    }

    void SpriteComponent::Update(f32){
        auto& item = proxy.GetRenderItem();
        item.transform = owner->GetTransform();
    }
}
