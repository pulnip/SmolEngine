#include "Actor.hpp"
#include "LinearAlgebra.hpp"
#include "MoveComponent.hpp"
#include "SpawnContext.hpp"

namespace Smol
{
    SMOL_COMPONENT(MoveComponent)
    SMOL_COMPONENT_END(MoveComponent)

    MoveComponent::MoveComponent(const SpawnContext& ctx)
        : transform(ctx.owner->GetTransform())
    {}

    void MoveComponent::Update(f32 dt) noexcept{
        transform.position += (speed * dt) * dir;
    }

    void MoveComponent::SetDirection(Vec3 dir) noexcept{
        auto length = normSquared(dir);

        if(length > 0.001f)
            dir = normalize(dir);

        this->dir = dir;
    }
}
