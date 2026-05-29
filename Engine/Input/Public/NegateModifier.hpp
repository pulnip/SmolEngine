#pragma once

#include "InputModifier.hpp"

namespace Smol
{
    class NegateModifier: public InputModifier{
    public:
        bool negateX = true, negateY = true, negateZ = true;

        InputValue Modify(InputValue v) noexcept override{
            if(negateX) v.raw.x = -v.raw.x;
            if(negateY) v.raw.y = -v.raw.y;
            if(negateZ) v.raw.z = -v.raw.z;

            return v;
        }
    };
}
