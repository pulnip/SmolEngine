#pragma once

#include "Component.hpp"
#include "InputAction.hpp"
#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class InputManager;

    class InputComponent: public TypedComponent<InputComponent>{
    private:
        InputManager& manager;
        std::vector<InputAction> actions;

    public:
        virtual ~InputComponent() = default;
        SMOL_DECLARE_PINNED(InputComponent)

        InputComponent(InputManager& manager);

        // Bind with Type erasure
        template<typename T>
        void BindAction(StrView action, TriggerEvent event,
            T* actor, void(T::*func)(InputValue)
        ){
            return bindAction(action, event,
                [actor, func](InputValue v){
                    (actor->*func)(v);
                }
            );
        }

    private:
        void bindAction(StrView action, TriggerEvent, InputAction::Callback&&);
    };
}
