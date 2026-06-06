#include <algorithm>
#include "Actor.hpp"
#include "Assert.hpp"
#include "ImageLoader.hpp"
#include "RHIDevice.hpp"
#include "RHITexture.hpp"
#include "SpawnContext.hpp"
#include "SpriteComponent.hpp"
#include "SpriteRenderer.hpp"

namespace{
    Smol::SpriteResource createSprite(
        Smol::RHIDevice& device,
        const Smol::DOM::Value& dom,
        const std::filesystem::path& contentRoot
    ){
        using namespace Smol;

        // TODO. move to Resource Manager
        auto image_path = dom.get<Str>("image_path");
        if(!image_path){
            return {};
        }

        auto imagePath = contentRoot / * image_path;
        auto image = loadImage(imagePath);

        auto texture = device.CreateTexture(
            RHITextureCreateDesc{
                .width = image.GetWidth(), .height = image.GetHeight(),
                .format = RHIPixelFormat::RGBA8_UNORM,
                .usage = RHITextureUsage::AllowShaderRead,
                .initialData = image.GetBufferPointer()
            }
        );
        return SpriteResource{
            .texture = std::move(texture),
            // TODO. parse asset meta file
            .frameCount = 8,
            .framePerSeconds = 0.16f
        };
    }
}

namespace Smol
{
    SMOL_COMPONENT(SpriteComponent)
    SMOL_COMPONENT_END(SpriteComponent)

    SpriteComponent::~SpriteComponent() = default;

    SpriteComponent::SpriteComponent(
        RHITextureRAII&& sprite,
        Transform& transform,
        SpriteRenderer& renderer
    )
        : resource(SpriteResource{
            .texture = std::move(sprite),
            .frameCount = 8,
            .framePerSeconds = 0.16f
        })
        , proxy(renderer.BindRenderItem(*sprite))
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
        : resource(createSprite(
            *ctx.device,
            ctx.dom,
            ctx.contentRoot
        ))
        , proxy(ctx.spriteRenderer->BindRenderItem(*resource.texture))
        , transform(ctx.owner->GetTransform())
    {
        SMOL_ASSERT(proxy.IsValid());

        auto& item = proxy.GetRenderItem();
        item.transform = transform;

        transform.scale = Vec3(4, 4, 1);
        // TODO. change later
        item.uvScale = {1.0f/16, 1.0f/16};
        item.offset = {0, 0};
    }

    void SpriteComponent::Update(f32 dt){
        elapsedTime += std::min(dt, resource.framePerSeconds);

        auto& item = proxy.GetRenderItem();
        item.transform = transform;

        if(elapsedTime > resource.framePerSeconds){
            elapsedTime -= resource.framePerSeconds;
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
        iframe = (iframe + 1) % resource.frameCount;
        synced = false;
    }
}
