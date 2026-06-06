#pragma once

#include "DOM.hpp"
#include "EnumUtil.hpp"
#include "Primitives.hpp"

namespace Smol
{
    template<typename T>
    struct TypeTraits;

    template<EnumType T>
    struct TypeTraits<T>{
        inline static CStr name = EnumTraits<T>::name;
        static void deserialize(void* data, const DOM::Value& value){
            if(auto v = value.asString()){
                *static_cast<T*>(data) = EnumTraits<T>::convert(*v);
            }
        }
    };

    template<>
    struct TypeTraits<bool>{
        static constexpr CStr name = "bool";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<i8>{
        static constexpr CStr name = "i8";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<i16>{
        static constexpr CStr name = "i16";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<i32>{
        static constexpr CStr name = "i32";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<i64>{
        static constexpr CStr name = "i64";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<u8>{
        static constexpr CStr name = "u8";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<u16>{
        static constexpr CStr name = "u16";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<u32>{
        static constexpr CStr name = "u32";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<u64>{
        static constexpr CStr name = "u64";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<usize>{
        static constexpr CStr name = "usize";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<isize>{
        static constexpr CStr name = "isize";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<f32>{
        static constexpr CStr name = "f32";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<f64>{
        static constexpr CStr name = "f64";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<Str>{
        static constexpr CStr name = "Str";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<Size2D>{
        static constexpr CStr name = "Size2D";
        static void deserialize(void*, const DOM::Value&);
    };

    template<>
    struct TypeTraits<Transform>{
        static constexpr CStr name = "Transform";
        static void deserialize(void*, const DOM::Value&);
    };

    struct TypeInfo{
        CStr name;
        usize size;
        void (*deserialize)(void*, const DOM::Value&);
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
