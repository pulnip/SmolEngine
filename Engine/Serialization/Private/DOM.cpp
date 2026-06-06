#include <charconv>
#include "DOM.hpp"

namespace{
    struct PathToken{
        Smol::StrView key;        // empty if pure index
        std::optional<size_t> index; // present if [N] follows
        bool valid = true;
    };

    PathToken nextToken(Smol::StrView& path){
        PathToken tok;
        if(path.empty()){
            tok.valid = false;
            return tok;
        }

        if(path.front() == '.'){
            path.remove_prefix(1);
        }

        // key 파싱 (있다면)
        if(!path.empty() && path.front() != '['){
            size_t end = 0;
            while(end < path.size() && path[end] != '.' && path[end] != '['){
                ++end;
            }

            tok.key = path.substr(0, end);
            path.remove_prefix(end);
        }

        // [N] 파싱 (있다면)
        if(!path.empty() && path.front() == '['){
            path.remove_prefix(1);
            size_t idx = 0;
            auto [p, ec] = std::from_chars(path.data(), path.data() + path.size(), idx);
            if(ec != std::errc{} || p == path.data() || p == path.data() + path.size() || *p != ']'){
                tok.valid = false;
                return tok;
            }

            tok.index = idx;
            path.remove_prefix(static_cast<size_t>(p - path.data()) + 1); // +1 for ']'
        }

        return tok;
    }
}


namespace Smol
{
    DOM::Value::Kind DOM::Value::kind() const noexcept{
        return std::visit([](auto&& v) -> Kind {
            using T = std::decay_t<decltype(v)>;

            if constexpr(std::is_same_v<T, bool>){
                return Kind::Bool;
            }
            else if constexpr(std::is_same_v<T, i64>){
                return Kind::Int;
            }
            else if constexpr(std::is_same_v<T, f64>){
                return Kind::Float;
            }
            else if constexpr(std::is_same_v<T, Str>){
                return Kind::String;
            }
            else if constexpr(std::is_same_v<T, ArrayRAII>){
                return Kind::Array;
            }
            else if constexpr(std::is_same_v<T, TableRAII>){
                return Kind::Table;
            }
            else{
                return Kind::None;
            }
        }, value);
    }

    const DOM::Value* DOM::Value::at(StrView path) const noexcept{
        const Value* cur = this;
        while(cur != nullptr && !path.empty()){
            auto tok = nextToken(path);
            if (!tok.valid){
                return nullptr;
            }

            if(!tok.key.empty()){
                auto tbl = cur->asTable();
                if(tbl == nullptr){
                    return nullptr;
                }

                auto it = tbl->find(Str(tok.key));
                if(it == tbl->end()){
                    return nullptr;
                }

                cur = &it->second;
            }
            if(tok.index){
                auto arr = cur->asArray();
                if(arr == nullptr || *tok.index >= arr->size()){
                    return nullptr;
                }

                cur = &(*arr)[*tok.index];
            }
        }

        return cur;
    }

    std::optional<bool> DOM::Value::asBool() const noexcept{
        if(auto p = std::get_if<bool>(&value)){
            return *p;
        }

        return std::nullopt;
    }
    std::optional<i64> DOM::Value::asInt() const noexcept{
        if(auto p = std::get_if<i64>(&value)){
            return *p;
        }

        return std::nullopt;
    }
    std::optional<f64> DOM::Value::asFloat() const noexcept{
        if(auto p = std::get_if<f64>(&value)){
            return *p;
        }

        return std::nullopt;
    }
    const Str* DOM::Value::asString() const noexcept{
        return std::get_if<Str>(&value);
    }
    const DOM::Array* DOM::Value::asArray() const noexcept{
        if(auto p = std::get_if<ArrayRAII>(&value)) return p->get();
        return nullptr;
    }
    const DOM::Table* DOM::Value::asTable() const noexcept{
        if(auto p = std::get_if<TableRAII>(&value)) return p->get();
        return nullptr;
    }

    template<>
    std::optional<bool> DOM::Value::get<bool>() const noexcept{
        return asBool();
    }

    template<>
    std::optional<i8> DOM::Value::get<i8>() const noexcept{
        if(auto opt = asInt()){
            return static_cast<i8>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<i16> DOM::Value::get<i16>() const noexcept{
        if(auto opt = asInt()){
            return static_cast<i16>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<i32> DOM::Value::get<i32>() const noexcept{
        if(auto opt = asInt()){
            return static_cast<i32>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<i64> DOM::Value::get<i64>() const noexcept{
        return asInt();
    }

    template<>
    std::optional<u8> DOM::Value::get<u8>() const noexcept{
        if(auto opt = asInt()){
            return static_cast<u8>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<u16> DOM::Value::get<u16>() const noexcept{
        if(auto opt = asInt()){
            return static_cast<u16>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<u32> DOM::Value::get<u32>() const noexcept{
        if(auto opt = asInt()){
            return static_cast<u32>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<u64> DOM::Value::get<u64>() const noexcept{
        if(auto opt = asInt()){
            return static_cast<u64>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<f32> DOM::Value::get<f32>() const noexcept{
        auto fopt = asFloat();
        auto iopt = asInt();
        if(fopt || iopt){
            return static_cast<f32>(fopt ? *fopt : static_cast<f32>(*iopt));
        }

        return std::nullopt;
    }

    template<>
    std::optional<f64> DOM::Value::get<f64>() const noexcept{
        auto fopt = asFloat();
        auto iopt = asInt();
        if(fopt || iopt){
            return fopt ? *fopt : static_cast<f64>(*iopt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<Str> DOM::Value::get<Str>() const noexcept{
        auto s = asString();
        if(s == nullptr){
            return std::nullopt;
        }
        return *s;
    }

    template<>
    std::optional<Vec2> DOM::Value::get<Vec2>() const noexcept{
        auto a = asArray();
        if(a == nullptr || a->size() != 2){
            return std::nullopt;
        }

        auto& e0 = (*a)[0];
        auto& e1 = (*a)[1];
        auto fx = e0.asFloat();
        auto ix = e0.asInt();
        auto fy = e1.asFloat();
        auto iy = e1.asInt();
        if(!(fx || ix) || !(fy || iy)){
            return std::nullopt;
        }

        auto x = fx ? *fx : static_cast<f32>(ix.value());
        auto y = fy ? *fy : static_cast<f32>(iy.value());
        return Vec2(x, y);
    }

    template<>
    std::optional<Vec3> DOM::Value::get<Vec3>() const noexcept{
        auto a = asArray();
        if(a == nullptr || a->size() != 3){
            return std::nullopt;
        }

        auto& e0 = (*a)[0];
        auto& e1 = (*a)[1];
        auto& e2 = (*a)[2];
        auto fx = e0.asFloat();
        auto ix = e0.asInt();
        auto fy = e1.asFloat();
        auto iy = e1.asInt();
        auto fz = e2.asFloat();
        auto iz = e2.asInt();
        if(!(fx || ix) || !(fy || iy) || !(fz || iz)){
            return std::nullopt;
        }

        auto x = fx ? *fx : static_cast<f32>(ix.value());
        auto y = fy ? *fy : static_cast<f32>(iy.value());
        auto z = fz ? *fz : static_cast<f32>(iz.value());
        return Vec3(x, y, z);
    }

    template<>
    std::optional<Vec4> DOM::Value::get<Vec4>() const noexcept{
        auto a = asArray();
        if(a == nullptr || a->size() != 4){
            return std::nullopt;
        }

        auto& e0 = (*a)[0];
        auto& e1 = (*a)[1];
        auto& e2 = (*a)[2];
        auto& e3 = (*a)[3];
        auto fx = e0.asFloat();
        auto ix = e0.asInt();
        auto fy = e1.asFloat();
        auto iy = e1.asInt();
        auto fz = e2.asFloat();
        auto iz = e2.asInt();
        auto fw = e3.asFloat();
        auto iw = e3.asInt();
        if(!(fx || ix) || !(fy || iy) || !(fz || iz) || !(fw || iw)){
            return std::nullopt;
        }

        auto x = fx ? *fx : static_cast<f32>(ix.value());
        auto y = fy ? *fy : static_cast<f32>(iy.value());
        auto z = fz ? *fz : static_cast<f32>(iz.value());
        auto w = fw ? *fw : static_cast<f32>(iw.value());
        return Vec4(x, y, z, w);
    }

    template<>
    std::optional<Size2D> DOM::Value::get<Size2D>() const noexcept{
        auto a = asArray();
        if(a == nullptr || a->size() != 4){
            return std::nullopt;
        }

        auto& e0 = (*a)[0];
        auto& e1 = (*a)[1];
        auto ix = e0.get<u32>();
        auto iy = e1.get<u32>();
        if(!ix || !iy){
            return std::nullopt;
        }
        return Size2D{*ix, *iy};
    }

    template<>
    std::optional<Transform> DOM::Value::get<Transform>() const noexcept{
        auto pos = get<Vec3>("position");
        auto rot = get<Vec3>("rotation");
        auto scale = get<Vec3>("scale");
        if(!pos || !rot || !scale){
            return std::nullopt;
        }

        return Transform{
            .position = *pos,
            .rotation = *rot,
            .scale = *scale
        };
    }
}
