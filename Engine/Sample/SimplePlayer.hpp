#pragma once

#include "ActorRegistry.hpp"
#include "InputValue.hpp"
#include "Pawn.hpp"

namespace Smol
{
    class SimplePlayer: public Pawn{
        SMOL_ACTOR_BODY(SimplePlayer, Pawn)

    private:
        SpriteComponent* spriteComponent = nullptr;
        Vec2 dir = Vec2{0, 0};

        // TODO. use SpriteAnimator
        f32 elapsedTime = 0.0f;

    public:
        ~SimplePlayer() = default;
        SMOL_DECLARE_MOVE_ONLY(SimplePlayer)

        SimplePlayer(SpawnContext&);

        void OnUpdate(f32) override;

        void SyncRenderState() override;

        void PossessedBy(CharacterController&) override;

    private:
        void OnMoveTriggered(InputValue);
        void OnMoveFinished(InputValue);
    };
}
