#pragma once

#include <functional>
#include "Component.hpp"
#include "GenericHandle.hpp"
#include "Geometry/Overlap2D.hpp"
#include "PhysicsEngine2D.hpp"
#include "Semantics.hpp"
#include "Shape2D.hpp"
#include "SlotMap.hpp"

namespace Smol
{
    class ColliderComponent: public TypedComponent<ColliderComponent>{
        SMOL_COMPONENT_BODY(ColliderComponent)

    private:
        using BeginOverlapCallback = std::function<void(
            ColliderComponent*,
            Actor*,
            ColliderComponent*,
            const OverlapResult2D&
        )>;
        using StayOverlapCallback = BeginOverlapCallback;
        using EndOverlapCallback = std::function<void(
            ColliderComponent*,
            Actor*,
            ColliderComponent*
        )>;
        SlotMap<BeginOverlapCallback> beginOverlapEvent;
        SlotMap<StayOverlapCallback> stayOverlapEvent;
        SlotMap<EndOverlapCallback> endOverlapEvent;

        // local transform of collider
        Transform2D transform;
        CollisionLayer layer = 0b0;
        CollisionMask mask = 0b0;
        PhysicsProxy proxy;

        bool debugFlag = true;

    public:
        ColliderComponent() = default;
        virtual ~ColliderComponent();
        SMOL_DECLARE_PINNED(ColliderComponent)

        void OnAttach();

        void Update(f32 dt) override;

        using BeginOverlapHandle = GenericHandle<BeginOverlapCallback>;
        using StayOverlapHandle = GenericHandle<StayOverlapCallback>;
        using EndOverlapHandle = GenericHandle<EndOverlapCallback>;

        template<std::derived_from<Actor> T>
        BeginOverlapHandle OnBeginOverlap(T* actor, void(T::*func)(
            ColliderComponent*,
            Actor*,
            ColliderComponent*,
            const OverlapResult2D&
        )){
            return beginOverlapEvent.Emplace([actor, func](
                ColliderComponent* myCollider,
                Actor* other,
                ColliderComponent* otherCollider,
                const OverlapResult2D& result
            ){
                (actor->*func)(myCollider, other, otherCollider, result);
            });
        }
        template<std::derived_from<Actor> T>
        StayOverlapHandle OnStayOverlap(T* actor, void(T::*func)(
            ColliderComponent*,
            Actor*,
            ColliderComponent*,
            const OverlapResult2D&
        )){
            return stayOverlapEvent.Emplace([actor, func](
                ColliderComponent* myCollider,
                Actor* other,
                ColliderComponent* otherCollider,
                const OverlapResult2D& result
            ){
                (actor->*func)(myCollider, other, otherCollider, result);
            });
        }
        template<std::derived_from<Actor> T>
        EndOverlapHandle OnEndOverlap(T* actor, void(T::*func)(
            ColliderComponent*,
            Actor*,
            ColliderComponent*
        )){
            return endOverlapEvent.Emplace([actor, func](
                ColliderComponent* myCollider,
                Actor* other,
                ColliderComponent* otherCollider
            ){
                (actor->*func)(myCollider, other, otherCollider);
            });
        }

        void UnbindBeginOverlap(BeginOverlapHandle handle){
            beginOverlapEvent.Remove(handle);
        }
        void UnbindStayOverlap(StayOverlapHandle handle){
            stayOverlapEvent.Remove(handle);
        }
        void UnbindEndOverlap(EndOverlapHandle handle){
            endOverlapEvent.Remove(handle);
        }

        void SetLayer(CollisionLayer layer){
            this->layer = layer;
        }

        void SetMask(CollisionMask mask){
            this->mask = mask;
        }

    private:
        RectCollider getWorldCollider() const;

    private:
        friend class World;

        void NotifyBeginOverlap(ColliderComponent* other, const OverlapResult2D& result);
        void NotifyStayOverlap(ColliderComponent* other, const OverlapResult2D& result);
        void NotifyEndOverlap(ColliderComponent* other);
    };
}
