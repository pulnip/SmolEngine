#pragma once

#include "Component.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class ColliderComponent: public TypedComponent<ColliderComponent>{
        SMOL_COMPONENT_BODY(ColliderComponent)

    public:
        ColliderComponent() = default;
        virtual ~ColliderComponent() = default;
        SMOL_DECLARE_PINNED(ColliderComponent)
    };
}
