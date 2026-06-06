#pragma once

#include "Component.hpp"

namespace Smol
{
    class Rigidbody: public TypedComponent<Rigidbody>{
        SMOL_COMPONENT_BODY(Rigidbody)

    };
}
