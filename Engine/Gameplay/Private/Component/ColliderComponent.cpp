#include "Actor.hpp"
#include "ColliderComponent.hpp"
#include "Collider2D.hpp"
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
            .shape = getWorldCollider(),
            .layer = layer,
            .mask = mask,
            .object = this
        });
    }

    void ColliderComponent::Update(f32 dt){
        // Sync to PhysicsEngine
        proxy.GetRef() = Collider2D{
            .shape = getWorldCollider(),
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

    RectCollider ColliderComponent::getWorldCollider() const{
        SMOL_ASSERT(owner != nullptr);
        const auto& t = owner->GetTransform();

        RectCollider c = collider;
        c.transform = static_cast<Transform2D>(owner->GetTransform()) * transform;

        return c;
    }

    void ColliderComponent::NotifyBeginOverlap(
        ColliderComponent* other,
        const OverlapResult2D& result
    ){
        for(const auto& callback: beginOverlapEvent){
            callback(this, other->GetOwner(), other, result);
        }
    }

    void ColliderComponent::NotifyStayOverlap(
        ColliderComponent* other,
        const OverlapResult2D& result
    ){
        for(const auto& callback: stayOverlapEvent){
            callback(this, other->GetOwner(), other, result);
        }
    }

    void ColliderComponent::NotifyEndOverlap(
        ColliderComponent* other
    ){
        for(const auto& callback: endOverlapEvent){
            callback(this, other->GetOwner(), other);
        }
    }
}
