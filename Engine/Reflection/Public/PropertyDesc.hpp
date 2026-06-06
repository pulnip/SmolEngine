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
    private:
        Member Class::* ptr = nullptr;

    public:
        explicit MemberAccessor(Member Class::* ptr)
            : ptr(ptr){}

        void* Get(void* obj) const override{
            Member& member = static_cast<Class*>(obj)->*ptr;
            return &member;
        }
    };

    template<typename Class, typename Member, typename Leaf>
    struct NestedMemberAccessor: public PropertyAccessor{
        Member Class::* memberPtr = nullptr;
        Leaf Member::* ptr = nullptr;

        explicit NestedMemberAccessor(
            Member Class::* memberPtr,
            Leaf Member::* ptr
        ): memberPtr(memberPtr), ptr(ptr){}

        void* Get(void* obj) const override{
            Member& member = static_cast<Class*>(obj)->*memberPtr;
            Leaf& leaf = (&member)->*ptr;
            return &leaf;
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
