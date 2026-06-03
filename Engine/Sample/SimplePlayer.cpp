#include <iostream>
#include "ImageLoader.hpp"
#include "RHIDevice.hpp"
#include "RHITexture.hpp"
#include "SimplePlayer.hpp"
#include "SpawnContext.hpp"
#include "SpriteComponent.hpp"

namespace Smol
{
    SMOL_ACTOR(SimplePlayer)
    SMOL_ACTOR_END(SimplePlayer)

    SimplePlayer::SimplePlayer(SpawnContext& context){
        SMOL_ASSERT(context.device != nullptr);
        SMOL_ASSERT(context.spriteRenderer != nullptr);

        // TODO.
        CStr IMAGE_PATH = "Content/Assets/Sprite/hollow_knight.png";
        auto image = loadImage(IMAGE_PATH);
        auto texture = context.device->CreateTexture(
            RHITextureCreateDesc{
                .width = image.GetWidth(), .height = image.GetHeight(),
                .format = RHIPixelFormat::RGBA8_UNORM,
                .usage = RHITextureUsage::AllowShaderRead,
                .initialData = image.GetBufferPointer()
            }
        );

        spriteComponent = AddComponent<SpriteComponent>(
            std::move(texture),
            *context.spriteRenderer
        );
        SMOL_ASSERT(spriteComponent != nullptr);
    }

    void SimplePlayer::PossessedBy(CharacterController& controller){
        Super::PossessedBy(controller);

        BindAction(
            "Move", TriggerEvent::Triggered,
            this, &SimplePlayer::OnMoveTriggered
        );
        BindAction(
            "Move", TriggerEvent::Finished,
            this, &SimplePlayer::OnMoveFinished
        );
    }

    void SimplePlayer::OnMoveTriggered(InputValue v){
        dir = v.GetAxis2D();
    }

    void SimplePlayer::OnMoveFinished(InputValue){
        dir = Vec2{0.0f, 0.0f};
    }

    void SimplePlayer::OnUpdate(f32 dt){
        elapsedTime += dt;
        constexpr f32 framePerSeconds = 0.16f;
        if(elapsedTime > framePerSeconds){
            elapsedTime -= framePerSeconds;
            spriteComponent->NextFrame();
        }
    }

    void SimplePlayer::SyncRenderState(){
        spriteComponent->SyncToRenderer();
    }
}
