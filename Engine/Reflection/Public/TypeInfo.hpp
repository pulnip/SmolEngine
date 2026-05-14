#pragma once

#include "Primitives.hpp"
#include "DOM.hpp"

namespace Smol
{
    struct TypeInfo{
        CStr name;
        usize size;
        void (*deserialize)(void*, const DOM::Value&);
    };

    template<typename T>
    struct TypeTraits;

    template<>
    struct TypeTraits<bool>{
        static constexpr CStr name = "bool";
        static void deserialize(void* data, const DOM::Value& value);
    };

    template<>
    struct TypeTraits<i32>{
        static constexpr CStr name = "i32";
        static void deserialize(void* data, const DOM::Value& value);
    };

    template<>
    struct TypeTraits<i64>{
        static constexpr CStr name = "i64";
        static void deserialize(void* data, const DOM::Value& value);
    };

    template<>
    struct TypeTraits<f32>{
        static constexpr CStr name = "f32";
        static void deserialize(void* data, const DOM::Value& value);
    };

    template<>
    struct TypeTraits<f64>{
        static constexpr CStr name = "f64";
        static void deserialize(void* data, const DOM::Value& value);
    };

    template<>
    struct TypeTraits<Str>{
        static constexpr CStr name = "Str";
        static void deserialize(void* data, const DOM::Value& value);
    };


    template<typename T>
    const TypeInfo* GetTypeInfo(){
        static const TypeInfo info{
            .name = TypeTraits<T>::name,
            .size = sizeof(T),
            .deserialize = &TypeTraits<T>::deserialize
        };
        return &info;
    }
}
