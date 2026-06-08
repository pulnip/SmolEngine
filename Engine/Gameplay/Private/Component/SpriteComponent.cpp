#include "Actor.hpp"
#include "Assert.hpp"
#include "ResourceManager.hpp"
#include "RHITexture.hpp"
#include "SpriteComponent.hpp"
#include "SpriteConfig.hpp"
#include "SpriteRenderer.hpp"
#include "World.hpp"

namespace Smol
{
    SMOL_COMPONENT(SpriteComponent)
    SMOL_COMPONENT_END(SpriteComponent)

    void SpriteComponent::OnAttach(
        const DOM::Value& dom,
        const std::filesystem::path& contentRoot
    ){
        SMOL_ASSERT(!handle.IsValid());
        const auto request = createSpriteRequest(dom, contentRoot);

        auto world = owner->GetWorld();
        auto spriteManager = world->GetSpriteManager();
        auto spriteRenderer = world->GetSpriteRenderer();

        handle = spriteManager->Load(request);
        proxy = spriteRenderer->BindRenderItem(handle);
        SMOL_ASSERT(proxy.IsValid());

        // Initialize RenderItem
        auto& item = proxy.GetRenderItem();
        item.transform = owner->GetTransform();

        auto spriteScale = dom.get<Vec2>("sprite_scale")
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
