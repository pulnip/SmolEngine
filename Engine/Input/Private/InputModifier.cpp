#include "InputModifier.hpp"
#include "DOM.hpp"
#include "LogLocal.hpp"
#include "StringUtil.hpp"

namespace{
    auto createNegateModifier(const Smol::DOM::Value& v){
        // negate all for default
        auto negateX = v.get<bool>("negate_x")
            .value_or(true);
        auto negateY = v.get<bool>("negate_y")
            .value_or(true);
        auto negateZ = v.get<bool>("negate_z")
            .value_or(true);

        auto anyNegate = negateX || negateY || negateZ;
        if(!anyNegate) [[unlikely]]
            LOG_WARN("No Negate in NegateModifier");

        return Smol::NegateModifier(negateX, negateY, negateZ);
    }

    auto createScaleModifier(const Smol::DOM::Value& v){
        using namespace Smol;

        if(auto factor = v.get<f32>("factor")){
            return ScaleModifier(*factor);
        }
        else if(auto factor = v.get<Vec3>("factor")){
            return ScaleModifier(*factor);
        }

        LOG_WARN("key \"factor\" not found in DOM::Value");
        return ScaleModifier();
    }

    auto toSwizzleOrder(Smol::StrView str){
        using namespace Smol;
        using enum SwizzleOrder;

        static StringHashMap<SwizzleOrder> map = {
            {"XYZ", XYZ},
            {"XZY", XZY},
            {"YXZ", YXZ},
            {"YZX", YZX},
            {"ZXY", ZXY},
            {"ZYX", ZYX}
        };

        auto upper = toUpper(str);
        if(auto it = map.find(upper); it != map.end()){
            return it->second;
        }

        LOG_ERROR("Unknown SwizzleOrder: {}", str);
        return XYZ;
    }

    auto createSwizzleModifier(const Smol::DOM::Value& v){
        using namespace Smol;

        if(auto order = v.get<Str>("order")){
            return SwizzleModifier(toSwizzleOrder(*order));
        }

        LOG_WARN("key \"order\" not found in DOM::Value");
        return SwizzleModifier();
    }
}

namespace Smol
{
    InputModifier CreateInputModifier(const DOM::Value& v){
        auto type = v.get<Str>("type");
        if(!type.has_value()){
            LOG_ERROR("key \"type\" not found in DOM::Value");
            return NoModifier{};
        }

        if(*type == "Negate"){
            return createNegateModifier(v);
        }
        else if(*type == "Scale"){
            return createScaleModifier(v);
        }
        else if(*type == "Swizzle"){
            return createSwizzleModifier(v);
        }
        else{
            LOG_ERROR("Unknown InputModifier: {}", *type);
            return NoModifier{};
        }
    }
}
