
#pragma once

#include "CharacterController.hpp"
#include "Pawn.hpp"

class PlayerCharacter: public Smol::Pawn{
    SMOL_ACTOR_BODY(PlayerCharacter, Smol::Pawn)

public:
    void PossessedBy(Smol::CharacterController&) override;

    void OnUpdate(float) override;

private:
    void OnMove(Smol::InputValue);
    void EndMove(Smol::InputValue);
    void OnJump(Smol::InputValue);
    void OnBowAim(Smol::InputValue);
    void EndBowAim(Smol::InputValue);
    void OnBowShoot(Smol::InputValue);

private:
    //TODO. 활 컴포넌트 추가


};
