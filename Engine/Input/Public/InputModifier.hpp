#pragma once

#include <concepts>
#include <variant>
#include "InputValue.hpp"
#include "Semantics.hpp"

namespace Smol
{
    namespace DOM
    {
        class Value;
    }

    namespace detail{
        template<typename T>
        concept InputModifier = requires(T t, InputValue v){
            { t.Modify(v) } -> std::same_as<InputValue>;
            { t(v) } -> std::same_as<InputValue>;
        };
    }

    class NoModifier final{
    public:
        NoModifier() = default;
        ~NoModifier() = default;
        SMOL_DECLARE_TRANSFERABLE(NoModifier)

        inline InputValue Modify(InputValue v) const noexcept{
            return v;
        }
        inline InputValue operator()(InputValue v) const noexcept{
            return Modify(v);
        }
    };

    class NegateModifier final{
    private:
        bool negateX = true, negateY = true, negateZ = true;

    public:
        NegateModifier() = default;
        ~NegateModifier() = default;
        SMOL_DECLARE_TRANSFERABLE(NegateModifier)

        NegateModifier(bool negateX, bool negateY, bool negateZ)
            : negateX(negateX), negateY(negateY), negateZ(negateZ){}

        inline InputValue Modify(InputValue v) const noexcept{
            auto raw = v.GetAxis3D();

            return InputValue(
                negateX ? -raw.x : raw.x,
                negateY ? -raw.y : raw.y,
                negateZ ? -raw.z : raw.z
            );
        }
        inline InputValue operator()(InputValue v) const noexcept{
            return Modify(v);
        }
    };
    static_assert(detail::InputModifier<NegateModifier>);

    enum SwizzleOrder: u8{
        XYZ = 0,
        XZY = 1,
        YXZ = 2,
        YZX = 3,
        ZXY = 4,
        ZYX = 5
    };

    class SwizzleModifier final{
    private:
        SwizzleOrder order = SwizzleOrder::XYZ;

    public:
        SwizzleModifier() = default;
        ~SwizzleModifier() = default;
        SMOL_DECLARE_TRANSFERABLE(SwizzleModifier)

        SwizzleModifier(SwizzleOrder order)
            : order(order) {}

        InputValue Modify(InputValue v) const noexcept{
            using enum SwizzleOrder;

            auto raw = v.GetAxis3D();

            switch(order){
            case XYZ:
                break;
            case XZY:
                raw = raw.xzy();
                break;
            case YXZ:
                raw = raw.yxz();
                break;
            case YZX:
                raw = raw.yzx();
                break;
            case ZXY:
                raw = raw.zxy();
                break;
            case ZYX:
                raw = raw.zyx();
                break;
            default:
                std::unreachable();
            }

            return raw;
        }
        inline InputValue operator()(InputValue v) const noexcept{
            return Modify(v);
        }
    };
    static_assert(detail::InputModifier<SwizzleModifier>);

    using InputModifier = std::variant<
        NoModifier,
        NegateModifier,
        SwizzleModifier
    >;
    InputModifier CreateInputModifier(const DOM::Value&);
    inline InputValue Apply(const InputModifier& modifier, InputValue value){
        return std::visit([value](const auto& modifier){
            return modifier.Modify(value);
        }, modifier);
    }
}
