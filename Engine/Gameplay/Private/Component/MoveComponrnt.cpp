#include <stdexcept>
#include "LinearAlgebra.hpp"
#include "MoveComponent.hpp"
#include "SpawnContext.hpp"

namespace Smol
{
    static Transform trs;

    SMOL_COMPONENT(MoveComponent)
    SMOL_COMPONENT_END(MoveComponent)

    MoveComponent::MoveComponent(const SpawnContext&)
        : transform(trs)
    {
        throw std::runtime_error("Unimplemented");
    }

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
