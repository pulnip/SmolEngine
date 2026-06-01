#include "Actor.hpp"
#include "InputComponent.hpp"
#include "Rigidbody.hpp"

namespace Smol
{
    template<>
    InputComponent* Actor::AddComponent<InputComponent>(InputManager& manager){
        constexpr auto index = GetComponentTypeIndex<InputComponent>();
        auto c = std::make_unique<InputComponent>(manager);
        builtinComponents[index] = std::move(c);

        return static_cast<InputComponent*>(builtinComponents[index].get());
    }

    template<>
    InputComponent* Actor::GetComponent<InputComponent>(){
        constexpr auto index = GetComponentTypeIndex<InputComponent>();
        return static_cast<InputComponent*>(builtinComponents[index].get());
    }

    template<>
    RigidBody* Actor::AddComponent<RigidBody>(){
        constexpr auto index = GetComponentTypeIndex<RigidBody>();
        auto c = std::make_unique<RigidBody>();
        builtinComponents[index] = std::move(c);

        return static_cast<RigidBody*>(builtinComponents[index].get());
    }

    template<>
    RigidBody* Actor::GetComponent<RigidBody>(){
        constexpr auto index = GetComponentTypeIndex<RigidBody>();
        return static_cast<RigidBody*>(builtinComponents[index].get());
    }
}
