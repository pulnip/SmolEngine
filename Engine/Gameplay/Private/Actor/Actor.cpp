#include "Actor.hpp"
#include "Assert.hpp"
#include "LogLocal.hpp"
#include "PtrUtil.hpp"
#include "World.hpp"
#include <utility>

namespace Smol
{
    SMOL_OBJECT(Actor)
        .SetProperty("transform", &Actor::transform)
        .SetProperty("position", &Actor::transform, &Transform::position)
        .SetProperty("rotation", &Actor::transform, &Transform::rotation)
        .SetProperty("scale", &Actor::transform, &Transform::scale)
    SMOL_OBJECT_END(Actor)

    Actor::~Actor() = default;

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

        auto* ptr = &transform.position;
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

    void Actor::AttachTo(Actor* p, bool keepWorld){
        if(p == this)
            return;

        // cycle check
        for(auto a = p; a != nullptr; a = a->GetParent()){
            if(a == this){
                return;
            }
        }

        auto t = keepWorld ? GetWorldTransform() : transform;

        // detach from old parent
        if(auto p = GetParent()){
            std::erase(p->children, handle);
        }

        if(p == nullptr){
            if(keepWorld){
                transform = t;
            }

            parent = Handle::InvalidHandle();
        }
        else{
            SMOL_ASSERT(p->handle.IsValid());

            if(keepWorld){
                // Notice. Expect parent has uniform scale
                transform = inverse(p->GetWorldTransform()) * t;
            }

            parent = p->handle;
            p->children.push_back(handle);
        }
    }

    Actor* Actor::GetParent() const noexcept{
        return parent.IsValid() ?
            world->TryGetActor(parent) :
            nullptr;
    }

    Transform Actor::GetWorldTransform() const noexcept{
        auto p = GetParent();
        SMOL_ASSERT(parent.IsValid() == (p != nullptr));

        return p != nullptr ?
            p->GetWorldTransform() * transform :
            transform;
    }

    Component* Actor::AddComponent(StrView type){
        auto object = ClassRegistry::Create(type);
        auto component = uniqueCast<Component>(object);
        if(component == nullptr) [[unlikely]]{
            LOG_WARN("Component type {} not exist", type);
            return nullptr;
        }
        component->MarkManaged(this);

        auto typeID = component->GetTypeID();
        auto ptr = component.get();
        userdefinedComponents[typeID] = std::move(component);

        static_cast<Component*>(ptr)->Init();

        return ptr;
    }

    void Actor::Destroy(bool cascade){
        if(!handle.IsValid()) [[unlikely]]{
            return;
        }

        // detach self at parent
        auto p = GetParent();
        if(p != nullptr){
            std::erase(p->children, handle);
        }

        // use snapshot for prevent side-effect on Actor::AttachTo
        auto snapshot = children;

        if(cascade){
            for(auto child: snapshot){
                if(auto c = world->TryGetActor(child))
                    c->Destroy(false);
            }
        }
        else{
            // attach to child's grand parent
            for(auto child: snapshot){
                if(auto c = world->TryGetActor(child)){
                    c->AttachTo(p, true);
                }
            }
        }

        if(handle.IsValid()){
            propagateDestroy();

            world->MarkDestroy(handle);
            // Guarantee Actor is not Destroyed twice
            handle = Handle::InvalidHandle();

            OnDestroy();
        }
    }

    void Actor::propagateDestroy(){
        for(auto& component: builtinComponents){
            if(component == nullptr)
                continue;

            component->Destroy();
        }

        for(auto& [_, component]: userdefinedComponents){
            SMOL_ASSERT(component != nullptr);

            component->Destroy();
        }
    }

    bool Actor::IsValid() const noexcept{
        SMOL_ASSERT((world != nullptr) == (handle.IsValid()));
        return world != nullptr && handle.IsValid();
    }

    void Actor::MarkManaged(World* world, Handle handle){
        SMOL_ASSERT(this->world == nullptr);
        this->world = world;
        this->handle = handle;
    }

    bool Actor::IsWorldShutdown() const noexcept{
        if(world == nullptr){
            return false;
        }

        return world->IsShutdown();
    }

    void Actor::detachChild(Handle handle){
        auto it = std::ranges::find(children, handle);
        children.erase(it);
    }
}
