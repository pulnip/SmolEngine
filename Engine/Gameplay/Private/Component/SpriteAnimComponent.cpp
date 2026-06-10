#include <algorithm>
#include "Actor.hpp"
#include "Assert.hpp"
#include "ResourceManager.hpp"
#include "RHITexture.hpp"
#include "SpriteAnimComponent.hpp"
#include "SpriteConfig.hpp"
#include "SpriteRenderer.hpp"
#include "World.hpp"

namespace Smol
{
    SMOL_COMPONENT(SpriteAnimComponent)
    SMOL_COMPONENT_END(SpriteAnimComponent)

    void SpriteAnimComponent::OnAttach(
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

        const auto& animations = request.animations;
        // TODO. change later
        if(auto it = animations.find("walk"); it != animations.end()){
            auto& animation = it->second;
            startRow = animation.startRow;
            startCol = animation.startCol;
            item.offset = {
                static_cast<float>(startCol),
                static_cast<float>(startRow)
            };
            frameCount = animation.frameCount;
            framePerSeconds = animation.frameDurationMs / 1000.0f;
        }
    }

    void SpriteAnimComponent::Update(f32 dt){
        elapsedTime += std::min(dt, framePerSeconds);

        auto& item = proxy.GetRenderItem();
        item.transform = owner->GetTransform();

        if(elapsedTime > framePerSeconds){
            elapsedTime -= framePerSeconds;
            nextFrame();
        }

        syncToRenderer();
    }

    void SpriteAnimComponent::syncToRenderer(){
        if(synced) return;

        auto& item = proxy.GetRenderItem();
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
