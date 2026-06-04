#include "Assert.hpp"
#include "CharacterController.hpp"
#include "ImageLoader.hpp"
#include "Log.hpp"
#include "RHIDevice.hpp"
#include "RHITexture.hpp"
#include "SimplePlayer.hpp"
#include "SpawnContext.hpp"
#include "SpriteComponent.hpp"
#include "World.hpp"

namespace{
    void ApplyPossessedBy(Smol::SimplePlayer& self, Smol::SpawnContext& ctx){
        SMOL_ASSERT(ctx.world != nullptr);

        using namespace Smol;

        auto name = ctx.dom.get<Str>("name");
        SMOL_ASSERT(name.has_value());
        auto controllerName = ctx.dom.get<Str>("possessed_by");

        if(!controllerName.has_value()){
            LOG_WARN("Engine",
                "No Controller to possess SimplePlayer \"{}\"",
                *name
            );
            return;
        }

        auto controllerActor = ctx.world->FindActorByName(*controllerName);
        if(controllerActor == nullptr){
            LOG_ERROR("Engine",
                "Controller {} not found. (Potentially invalid Actor Define Order)",
                *controllerName
            );
            return;
        }

        auto controller = dynamic_cast<CharacterController*>(controllerActor);
        if(controller == nullptr){
            LOG_ERROR("Engine",
                "Actor {} is not CharacterController",
                *controllerName
            );
            return;
        }

        self.PossessedBy(*controller);
    }

    auto ApplySpriteComponent(Smol::SimplePlayer& self, Smol::SpawnContext& ctx){
        SMOL_ASSERT(ctx.device != nullptr);
        SMOL_ASSERT(ctx.spriteRenderer != nullptr);

        using namespace Smol;

        // TODO.
        CStr IMAGE_PATH = "Content/Assets/Sprite/hollow_knight.png";
        auto image = loadImage(IMAGE_PATH);
        auto texture = ctx.device->CreateTexture(
            RHITextureCreateDesc{
                .width = image.GetWidth(), .height = image.GetHeight(),
                .format = RHIPixelFormat::RGBA8_UNORM,
                .usage = RHITextureUsage::AllowShaderRead,
                .initialData = image.GetBufferPointer()
            }
        );

        auto spriteComponent = self.AddComponent<SpriteComponent>(
            std::move(texture),
            *ctx.spriteRenderer
        );
        SMOL_ASSERT(spriteComponent != nullptr);

        return spriteComponent;
    }
}

namespace Smol
{
    SMOL_ACTOR(SimplePlayer)
    SMOL_ACTOR_END(SimplePlayer)

    SimplePlayer::SimplePlayer(SpawnContext& ctx){
        ApplyPossessedBy(*this, ctx);
        spriteComponent = ApplySpriteComponent(*this, ctx);
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
