#pragma once

#include "Actor.hpp"
#include "InputComponent.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class CharacterController;

    class Pawn: public Actor{
        SMOL_ACTOR_BODY(Pawn)

    private:
        InputComponent* possessor = nullptr;

    public:
        Pawn() = default;
        ~Pawn();
        Pawn(Pawn&& other)
            : possessor(other.possessor)
        {
            other.possessor = nullptr;
        }
        Pawn& operator=(Pawn&& other){
            possessor = other.possessor;
            other.possessor = nullptr;

            return *this;
        }
        SMOL_DECLARE_NON_COPYABLE(Pawn)

        // Notice! Should be called before BindAction
        virtual void PossessedBy(CharacterController&);
        void Unpossess();

    protected:
        // Bind with Type erasure
        template<typename T>
        void BindAction(StrView action, TriggerEvent event,
            T* pawn, void(T::*func)(InputValue)
        ){
            if(possessor != nullptr) [[likely]] {
                possessor->BindAction(action, event, this,
                    [pawn, func](InputValue v){ (pawn->*func)(v); }
                );
            }
        }
    };
}
