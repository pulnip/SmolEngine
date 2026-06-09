#pragma once

#include "Primitives.hpp"
#include "Shape2D.hpp"

namespace Smol
{
    using CollisionLayer = u32;
    using CollisionMask = CollisionLayer;
    class Object;

    struct Collider2D{
        Shape2D shape;
        CollisionLayer layer = 0b1;
        CollisionMask mask = 0b1;
        // ColliderComponent
        Object* object = nullptr;
    };
}
