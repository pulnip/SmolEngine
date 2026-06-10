#include "Actor.hpp"
#include "ColliderComponent.hpp"
#include "Collider2D.hpp"
#include "LogLocal.hpp"
#include "Primitives.hpp"
#include "World.hpp"

namespace Smol
{
    SMOL_COMPONENT(ColliderComponent)
        .SetProperty("position", &ColliderComponent::transform, &Transform2D::position)
        .SetProperty("scale", &ColliderComponent::transform, &Transform2D::scale)
        .SetProperty("layer", &ColliderComponent::layer)
        .SetProperty("mask", &ColliderComponent::mask)
    SMOL_COMPONENT_END(ColliderComponent)

    void ColliderComponent::OnAttach(){
        SMOL_ASSERT(owner != nullptr);

        auto& physics = owner->GetWorld()->GetPhysics();
        proxy = physics.BindCollider(Collider2D{
            .shape = collider,
            .layer = layer,
            .mask = mask,
            .object = this
        });
    }

    void ColliderComponent::Update(f32 dt){
        const auto& t = owner->GetTransform();
        collider.transform = static_cast<Transform2D>(owner->GetTransform()) * transform;

        // Sync to PhysicsEngine
        proxy.GetItem() = Collider2D{
            .shape = collider,
            .layer = layer,
            .mask = mask,
            .object = this
        };
    }

    ColliderComponent::~ColliderComponent(){
        SMOL_ASSERT(owner != nullptr);

        if(owner->IsWorldShutdown())
            return;

        auto& physics = owner->GetWorld()->GetPhysics();
    }

    void ColliderComponent::NotifyBeginOverlap(
        ColliderComponent* other,
        const OverlapResult2D& result
    ){

    }

    void ColliderComponent::NotifyStayOverlap(
        ColliderComponent* other,
        const OverlapResult2D& result
    ){

    }

    void ColliderComponent::NotifyEndOverlap(
        ColliderComponent* other
    ){

    }
}
