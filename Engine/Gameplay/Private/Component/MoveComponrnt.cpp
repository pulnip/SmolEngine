#include "LinearAlgebra.hpp"
#include "MoveComponent.hpp"

namespace Smol
{
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
