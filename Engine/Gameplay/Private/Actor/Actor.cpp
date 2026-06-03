#include "Actor.hpp"
#include "InputComponent.hpp"
#include "Rigidbody.hpp"
#include "SpriteComponent.hpp"

namespace Smol
{
    template<>
    InputComponent* Actor::AddComponent<InputComponent>(IInputManager& manager){
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

    template<>
    SpriteComponent* Actor::AddComponent<SpriteComponent>(
        RHITextureRAII&& texture,
        SpriteRenderer& renderer
    ){
        constexpr auto index = GetComponentTypeIndex<SpriteComponent>();
        auto c = std::make_unique<SpriteComponent>(std::move(texture), renderer);
        builtinComponents[index] = std::move(c);

        return static_cast<SpriteComponent*>(builtinComponents[index].get());
    }

    template<>
    SpriteComponent* Actor::GetComponent<SpriteComponent>(){
        constexpr auto index = GetComponentTypeIndex<SpriteComponent>();
        return static_cast<SpriteComponent*>(builtinComponents[index].get());
    }
}
