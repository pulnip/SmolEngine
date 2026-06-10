#pragma once

#include "Primitives.hpp"
#include "Shape2D.hpp"

namespace Smol
{
    // 레이어(들)에 있는 물체와의 충돌 검사
    using CollisionLayer = u32;
    // 충돌 검사 결과를 통지받을지 여부
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
