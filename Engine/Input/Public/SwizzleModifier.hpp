#pragma once

#include <utility>
#include "InputModifier.hpp"
#include "Primitives.hpp"

namespace Smol
{
    enum SwizzleOrder: u8{
        XYZ = 0,
        XZY = 1,
        YXZ = 2,
        YZX = 3,
        ZXY = 4,
        ZYX = 5
    };

    class SwizzleModifier: public InputModifier{
    public:
        SwizzleOrder order = SwizzleOrder::XYZ;

        InputValue Modify(InputValue v) noexcept override{
            using enum SwizzleOrder;

            switch(order){
            case XYZ:
                break;
            case XZY:
                v.raw = v.raw.xzy();
                break;
            case YXZ:
                v.raw = v.raw.yxz();
                break;
            case YZX:
                v.raw = v.raw.yzx();
                break;
            case ZXY:
                v.raw = v.raw.zxy();
                break;
            case ZYX:
                v.raw = v.raw.zxy();
                break;
            default:
                std::unreachable();
            }

            return v;
        }
    };
}
