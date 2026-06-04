#include "CharacterController.hpp"
#include "Pawn.hpp"

namespace Smol
{
    SMOL_ACTOR(Pawn)
    SMOL_ACTOR_END(Pawn)

    Pawn::~Pawn(){
        if(!IsWorldShutdown()){
            Unpossess();
        }
    }

    void Pawn::PossessedBy(CharacterController& controller){
        possessor = &controller.GetInputComponent();
    }

    void Pawn::Unpossess(){
        if(possessor != nullptr){
            possessor->UnbindAction(this);
            possessor = nullptr;
        }
    }
}
