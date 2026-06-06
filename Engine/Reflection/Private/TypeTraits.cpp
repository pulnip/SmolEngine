#include "TypeInfo.hpp"

namespace Smol
{
    void TypeTraits<bool>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asBool()){
            *static_cast<bool*>(data) = *v;
        }
    }

    void TypeTraits<i8>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asInt()){
            *static_cast<i8*>(data) = static_cast<i8>(*v);
        }
    }

    void TypeTraits<i16>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asInt()){
            *static_cast<i16*>(data) = static_cast<i16>(*v);
        }
    }

    void TypeTraits<i32>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asInt()){
            *static_cast<i32*>(data) = static_cast<i32>(*v);
        }
    }

    void TypeTraits<i64>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asInt()){
            *static_cast<i64*>(data) = *v;
        }
    }

    void TypeTraits<u8>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asInt()){
            *static_cast<u8*>(data) = static_cast<u8>(*v);
        }
    }

    void TypeTraits<u16>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asInt()){
            *static_cast<u16*>(data) = static_cast<u16>(*v);
        }
    }

    void TypeTraits<u32>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asInt()){
            *static_cast<u32*>(data) = static_cast<u32>(*v);
        }
    }

    void TypeTraits<u64>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asInt()){
            *static_cast<u64*>(data) = static_cast<u64>(*v);
        }
    }

    void TypeTraits<f32>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asFloat()){
            *static_cast<f32*>(data) = static_cast<f32>(*v);
        }
    }

    void TypeTraits<f64>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asFloat()){
            *static_cast<f64*>(data) = *v;
        }
    }

    void TypeTraits<Str>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asString()){
            *static_cast<Str*>(data) = *v;
        }
    }

    void TypeTraits<Size2D>::deserialize(void* data, const DOM::Value& value){
        auto& size = *static_cast<Size2D*>(data);

        if(auto v = value.get<u32>("x"); v.has_value()){
            size.x = *v;
        }
        if(auto v = value.get<u32>("y"); v.has_value()){
            size.y = *v;
        }
    }

    void TypeTraits<Transform>::deserialize(void* data, const DOM::Value& value){
        auto& transform = *static_cast<Transform*>(data);

        if(auto v = value.get<Vec3>("position"); v.has_value()){
            transform.position = *v;
        }
        if(auto v = value.get<Vec4>("rotation"); v.has_value()){
            transform.rotation = *v;
        }
        if(auto v = value.get<Vec3>("scale"); v.has_value()){
            transform.scale = *v;
        }
    }
}
