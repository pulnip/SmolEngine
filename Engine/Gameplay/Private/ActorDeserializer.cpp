#include "Actor.hpp"
#include "ActorDeserializer.hpp"
#include "CharacterController.hpp"
#include "ClassRegistry.hpp"
#include "DOM.hpp"
#include "ImageLoader.hpp"
#include "InputComponent.hpp"
#include "LogLocal.hpp"
#include "Pawn.hpp"
#include "RHIDevice.hpp"
#include "RHITexture.hpp"
#include "World.hpp"

namespace{
    template <typename Derived, typename Base>
        requires std::is_base_of_v<Base, Derived>
    std::unique_ptr<Derived> uniqueCast(std::unique_ptr<Base>& base){
        if(Derived* raw = dynamic_cast<Derived*>(base.get())){
            base.release();
            return std::unique_ptr<Derived>(raw);
        }

        return nullptr;
    }

    void applyInputComponent(Smol::Actor& actor, const Smol::SpawnContext& ctx){
        using namespace Smol;

        actor.AddComponent<InputComponent>(ctx.inputManager);
    }

    void applySpriteComponent(Smol::Actor& actor, const Smol::SpawnContext& ctx){
        SMOL_ASSERT(ctx.device != nullptr);
        SMOL_ASSERT(ctx.spriteRenderer != nullptr);

        using namespace Smol;

        auto image_path = ctx.dom.get<Str>("image_path");
        if(!image_path){
            return;
        }

        auto imagePath = ctx.contentRoot / * image_path;

        auto image = loadImage(imagePath);
        auto texture = ctx.device->CreateTexture(
            RHITextureCreateDesc{
                .width = image.GetWidth(), .height = image.GetHeight(),
                .format = RHIPixelFormat::RGBA8_UNORM,
                .usage = RHITextureUsage::AllowShaderRead,
                .initialData = image.GetBufferPointer()
            }
        );

        actor.AddComponent<SpriteComponent>(
            std::move(texture),
            *ctx.spriteRenderer
        );
    }

    void applyComponent(Smol::Actor& actor, const Smol::SpawnContext& ctx){
        auto type = ctx.dom.get<Smol::Str>("type");
        if(!type.has_value()) [[unlikely]]{
            LOG_ERROR("no \"type\" key in [[actors.component]]");
            return;
        }

        if(*type == "InputComponent"){
            applyInputComponent(actor, ctx);
        }
        else if(*type == "SpriteComponent"){
            applySpriteComponent(actor, ctx);
        }
    }

    void apply(Smol::Actor& actor, const Smol::SpawnContext& ctx){
        using namespace Smol;

        ctx.dom.forEach("components", [&](const DOM::Value& node){
            auto nodeContext = ctx.WithDOM(node);
            applyComponent(actor, nodeContext);
        });
    }

    void applyPossessedBy(
        Smol::Pawn& pawn,
        Smol::StrView possessor,
        Smol::World& world
    ){
        using namespace Smol;

        auto actor = world.FindActorByName(possessor);
        if(actor == nullptr){
            LOG_ERROR("Engine",
                "Controller {} not found. (Potentially invalid Actor Define Order)",
                possessor
            );
            return;
        }

        auto controller = dynamic_cast<CharacterController*>(actor);
        if(controller == nullptr){
            LOG_ERROR("Engine",
                "Actor {} is not CharacterController",
                possessor
            );
            return;
        }

        pawn.PossessedBy(*controller);
    }

    void apply(Smol::Pawn& pawn, const Smol::SpawnContext& ctx){
        using namespace Smol;

        apply(static_cast<Actor&>(pawn), ctx);

        auto possessedBy = ctx.dom.get<Str>("possessed_by");
        if(possessedBy.has_value()){
            SMOL_ASSERT(ctx.world != nullptr);

            applyPossessedBy(pawn, *possessedBy, *ctx.world);
        }
    }
}

namespace Smol
{
    ActorRAII CreateActor(StrView type, const SpawnContext& ctx){
        auto object = CreateObject(type, ctx);

        if(object->IsA("Pawn")){
            auto pawn = uniqueCast<Pawn>(object);
            apply(*pawn, ctx);

            return pawn;
        }
        else if(object->IsA("Actor")){{
            auto actor = uniqueCast<Actor>(object);
            apply(*actor, ctx);

            return actor;
        }}
        else{
            return nullptr;
        }
    }
}
