#include "Actor.hpp"
#include "ActorDeserializer.hpp"
#include "CharacterController.hpp"
#include "ClassRegistry.hpp"
#include "DOM.hpp"
#include "InputComponent.hpp"
#include "LogLocal.hpp"
#include "MoveComponent.hpp"
#include "SpriteComponent.hpp"
#include "Pawn.hpp"
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

    void applyComponent(Smol::Actor& actor, const Smol::SpawnContext& ctx){
        SMOL_ASSERT(ctx.owner == &actor);

        using namespace Smol;

        auto type = ctx.dom.get<Str>("type");
        if(!type.has_value()) [[unlikely]]{
            LOG_ERROR("no \"type\" key in [[actors.component]]");
            return;
        }


        if(*type == "InputComponent"){
            actor.AddComponent<InputComponent>(ctx);
        }
        else if(*type == "MoveComponent"){
            actor.AddComponent<MoveComponent>(ctx);
        }
        else if(*type == "SpriteComponent"){
            SMOL_ASSERT(ctx.device != nullptr);
            SMOL_ASSERT(ctx.spriteRenderer != nullptr);

            actor.AddComponent<SpriteComponent>(ctx);
        }
    }

    void apply(Smol::Actor& actor, const Smol::SpawnContext& ctx){
        using namespace Smol;

        ctx.dom.forEach("components", [&](const DOM::Value& node){
            auto nodeContext = ctx.WithDOM(node);
            applyComponent(actor, nodeContext);
        });

        ApplyProperties(
            ClassRegistry::Get().DescFor<Actor>(),
            &actor,
            ctx.dom
        );
    }

    void applyPossessedBy(
        Smol::Pawn& pawn,
        Smol::StrView possessor,
        Smol::World& world
    ){
        using namespace Smol;

        auto actor = world.FindActorByName(possessor);
        if(actor == nullptr){
            LOG_ERROR("Controller {} not found. (Potentially invalid Actor Define Order)",
                possessor
            );
            return;
        }

        auto controller = dynamic_cast<CharacterController*>(actor);
        if(controller == nullptr){
            LOG_ERROR("Actor {} is not CharacterController",
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

            auto nodeContext = ctx.WithOwner(pawn.get());
            apply(*pawn, nodeContext);

            return pawn;
        }
        else if(object->IsA("Actor")){{
            auto actor = uniqueCast<Actor>(object);

            auto nodeContext = ctx.WithOwner(actor.get());
            apply(*actor, nodeContext);

            return actor;
        }}
        else{
            return nullptr;
        }
    }
}
