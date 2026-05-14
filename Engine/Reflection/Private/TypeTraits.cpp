#include "TypeInfo.hpp"

namespace Smol
{
    void TypeTraits<bool>::deserialize(void* data, const DOM::Value& value){
        if(auto v = value.asBool()){
            *static_cast<bool*>(data) = *v;
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
}
