#pragma once

#include <optional>
#include "Primitives.hpp"
#include "TypeInfo.hpp"

namespace Smol
{
    struct PropertyAccessor{
        virtual ~PropertyAccessor() = default;
        virtual void* Get(void* obj) const = 0;
    };

    template<typename Class, typename Member>
    struct MemberAccessor: public PropertyAccessor{
        Member Class::* ptr = nullptr;

        explicit MemberAccessor(Member Class::* ptr):ptr(ptr){}

        void* Get(void* obj) const override{
            return &(static_cast<Class*>(obj)->*ptr);
        }
    };

    struct PropertyMeta{
        std::optional<std::pair<f32, f32>> range = std::nullopt;
        CStr tooltip = nullptr;
    };

    struct PropertyDesc{
        const TypeInfo& typeInfo;
        RAII<PropertyAccessor> accessor;
        PropertyMeta meta;
    };
}