#include "Actor.hpp"
#include "Assert.hpp"
#include "LinearAlgebra.hpp"
#include "MoveComponent.hpp"
#include "SpawnContext.hpp"

namespace Smol
{
    SMOL_COMPONENT(MoveComponent)
    SMOL_COMPONENT_END(MoveComponent)

    MoveComponent::MoveComponent(const SpawnContext& ctx)
        : TypedComponent(ctx.owner)
    {
        SMOL_ASSERT(owner != nullptr);
    }

    void MoveComponent::Update(f32 dt) noexcept{
        auto& transform = owner->GetTransform();
        transform.position += (speed * dt) * dir;
    }

    void MoveComponent::SetDirection(Vec3 dir) noexcept{
        auto length = normSquared(dir);

        if(length > 0.001f)
            dir = normalize(dir);

        this->dir = dir;
    }
}
