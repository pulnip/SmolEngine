
#pragma once

#include "CharacterController.hpp"
#include "Pawn.hpp"
#include "Primitives.hpp"

class PlayerCharacter: public Smol::Pawn{
    SMOL_ACTOR_BODY(PlayerCharacter, Smol::Pawn)

public:
    void PossessedBy(Smol::CharacterController&) override;

    void OnStart() override;

    void OnUpdate(float) override;

private:
    void StartMove(Smol::InputValue);
    void OnMove(Smol::InputValue);
    void EndMove(Smol::InputValue);
    void OnJump(Smol::InputValue);
    void StartBowAim(Smol::InputValue);
    void OnBowAim(Smol::InputValue);
    void EndBowAim(Smol::InputValue);
    void OnBowShoot(Smol::InputValue);

private:
    Smol::Vec3 startPos;

};
