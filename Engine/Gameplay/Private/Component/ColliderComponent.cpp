#include "Actor.hpp"
#include "ColliderComponent.hpp"
#include "Collider2D.hpp"
#include "LinearAlgebra.hpp"
#include "World.hpp"

namespace Smol
{
    SMOL_COMPONENT(ColliderComponent)
    SMOL_COMPONENT_END(ColliderComponent)

    void ColliderComponent::OnAttach(){
        SMOL_ASSERT(owner != nullptr);

        auto& physics = owner->GetWorld()->GetPhysics();
        proxy = physics.BindCollider(Collider2D{
            .shape = collider,
            .layer = 0b1,
            .mask = 0b1,
            .object = this
        });
    }

    void ColliderComponent::Update(f32 dt){
        const auto& transform = owner->GetTransform();
        collider.transform = Transform2D{
            .position = static_cast<Vec2>(transform.position),
            .theta = extractZRot(transform.rotation),
            .scale = static_cast<Vec2>(transform.scale)
        };

        // Sync to PhysicsEngine
        auto& item = proxy.GetItem();
        item.shape = collider;
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
