#include "Actor.hpp"
#include "InputComponent.hpp"
#include "MoveComponent.hpp"
#include "Rigidbody.hpp"
#include "SpriteComponent.hpp"
#include "World.hpp"

namespace Smol
{
    SMOL_OBJECT(Actor)
        .SetProperty("transform", &Actor::transform)
        .SetProperty("position", &Actor::transform, &Transform::position)
        .SetProperty("rotation", &Actor::transform, &Transform::rotation)
        .SetProperty("scale", &Actor::transform, &Transform::scale)
    SMOL_OBJECT_END(Actor)

    Actor::Actor(Actor&& other) noexcept
        : builtinComponents(std::move(other.builtinComponents))
        , userdefinedComponents(std::move(other.userdefinedComponents))
        , children(std::move(other.children))
        , transform(std::move(other.transform))
        , world(other.world)
        , handle(other.handle)
    {
        other.world = nullptr;
        other.handle = Handle::InvalidHandle();
    }

    Actor& Actor::operator=(Actor&& other) noexcept{
        builtinComponents = std::move(other.builtinComponents);
        userdefinedComponents = std::move(other.userdefinedComponents);
        children = std::move(other.children);
        transform = std::move(other.transform);
        world = other.world;
        handle = other.handle;

        other.world = nullptr;
        other.handle = Handle::InvalidHandle();

        return *this;
    }

    void Actor::Update(f32 dt){
        updateComponents(dt);

        OnUpdate(dt);
    }

    void Actor::updateComponents(f32 dt){
        for(auto& component: builtinComponents){
            if(component == nullptr)
                continue;

            component->Update(dt);
        }

        for(auto& [_, component]: userdefinedComponents){
            SMOL_ASSERT(component != nullptr);

            component->Update(dt);
        }
    }

    void Actor::Destroy(){
        if(world == nullptr) [[unlikely]]{
            return;
        }

        SMOL_ASSERT(handle.IsValid());
        world->MarkDestroy(handle);

        // Guarantee Actor is not Destroyed twice
        world = nullptr;
        handle = Handle::InvalidHandle();
    }

    void Actor::MarkManaged(World* world, Handle handle){
        this->world = world;
        this->handle = handle;
    }

    bool Actor::IsWorldShutdown() const noexcept{
        if(world == nullptr){
            return false;
        }

        return world->IsShutdown();
    }

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
    MoveComponent* Actor::AddComponent<MoveComponent>(){
        constexpr auto index = GetComponentTypeIndex<MoveComponent>();
        auto c = std::make_unique<MoveComponent>(transform);
        builtinComponents[index] = std::move(c);

        return static_cast<MoveComponent*>(builtinComponents[index].get());
    }

    template<>
    MoveComponent* Actor::GetComponent<MoveComponent>(){
        constexpr auto index = GetComponentTypeIndex<MoveComponent>();
        return static_cast<MoveComponent*>(builtinComponents[index].get());
    }

    template<>
    Rigidbody* Actor::AddComponent<Rigidbody>(){
        constexpr auto index = GetComponentTypeIndex<Rigidbody>();
        auto c = std::make_unique<Rigidbody>();
        builtinComponents[index] = std::move(c);

        return static_cast<Rigidbody*>(builtinComponents[index].get());
    }

    template<>
    Rigidbody* Actor::GetComponent<Rigidbody>(){
        constexpr auto index = GetComponentTypeIndex<Rigidbody>();
        return static_cast<Rigidbody*>(builtinComponents[index].get());
    }

    template<>
    SpriteComponent* Actor::AddComponent<SpriteComponent>(
        RHITextureRAII&& texture,
        SpriteRenderer& renderer
    ){
        constexpr auto index = GetComponentTypeIndex<SpriteComponent>();
        auto c = std::make_unique<SpriteComponent>(std::move(texture), transform, renderer);
        builtinComponents[index] = std::move(c);

        return static_cast<SpriteComponent*>(builtinComponents[index].get());
    }

    template<>
    SpriteComponent* Actor::GetComponent<SpriteComponent>(){
        constexpr auto index = GetComponentTypeIndex<SpriteComponent>();
        return static_cast<SpriteComponent*>(builtinComponents[index].get());
    }
}
