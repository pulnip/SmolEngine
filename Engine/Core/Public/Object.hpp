#pragma once

#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class Object{
    public:
        Object() = default;
        virtual ~Object() = default;
        SMOL_DECLARE_MOVE_ONLY(Object)

        static constexpr CStr StaticClassName(){
            return "Object";
        }
        virtual CStr GetClassName() const{
            return StaticClassName();
        }

        virtual bool IsA(StrView name) const{
            return name == StaticClassName();
        }
    };
}
