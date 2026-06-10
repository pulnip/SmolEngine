#pragma once

#include "Component.hpp"
#include "Geometry/Overlap2D.hpp"
#include "PhysicsEngine2D.hpp"
#include "Shape2D.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class ColliderComponent: public TypedComponent<ColliderComponent>{
        SMOL_COMPONENT_BODY(ColliderComponent)

    private:
        RectCollider collider;
        PhysicsProxy proxy;

    public:
        ColliderComponent() = default;
        virtual ~ColliderComponent();
        SMOL_DECLARE_PINNED(ColliderComponent)

        void OnAttach();

        void Update(f32 dt) override;

        void NotifyBeginOverlap(ColliderComponent* other, const OverlapResult2D& result);
        void NotifyStayOverlap(ColliderComponent* other, const OverlapResult2D& result);
        void NotifyEndOverlap(ColliderComponent* other);
    };
}
