#pragma once

#include "Component.hpp"
#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class MoveComponent: public TypedComponent<MoveComponent>{
        SMOL_COMPONENT_BODY(MoveComponent)

    private:
        Vec3 dir;
        float speed = 12.0f;

    public:
        MoveComponent() = default;
        virtual ~MoveComponent() = default;
        SMOL_DECLARE_PINNED(MoveComponent)

        void Update(f32) noexcept override;

        void SetDirection(Vec3 dir) noexcept;
    };
}
