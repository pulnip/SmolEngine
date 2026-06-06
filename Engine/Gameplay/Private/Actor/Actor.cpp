#include "Actor.hpp"
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
}
