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
                auto* tbl = cur->asTable();
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
                auto* arr = cur->asArray();
                if(arr == nullptr || *tok.index >= arr->size()){
                    return nullptr;
                }

                cur = &(*arr)[*tok.index];
            }
        }

        return cur;
    }

    std::optional<bool> DOM::Value::asBool() const noexcept{
        if(auto* p = std::get_if<bool>(&value)){
            return *p;
        }

        return std::nullopt;
    }
    std::optional<i64> DOM::Value::asInt() const noexcept{
        if(auto* p = std::get_if<i64>(&value)){
            return *p;
        }

        return std::nullopt;
    }
    std::optional<f64> DOM::Value::asFloat() const noexcept{
        if(auto* p = std::get_if<f64>(&value)){
            return *p;
        }

        return std::nullopt;
    }
    const Str* DOM::Value::asString() const noexcept{
        return std::get_if<Str>(&value);
    }
    const DOM::Array* DOM::Value::asArray() const noexcept{
        if(auto* p = std::get_if<ArrayRAII>(&value)) return p->get();
        return nullptr;
    }
    const DOM::Table* DOM::Value::asTable() const noexcept{
        if(auto* p = std::get_if<TableRAII>(&value)) return p->get();
        return nullptr;
    }

    template<>
    std::optional<bool> DOM::Value::get<bool>(StrView p) const noexcept{
        auto* n = at(p);

        return n ? n->asBool() : std::nullopt;
    }

    template<>
    std::optional<i8> DOM::Value::get<i8>(StrView p) const noexcept{
        auto* n = at(p);

        if(auto opt = n->asInt()){
            return static_cast<i8>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<i16> DOM::Value::get<i16>(StrView p) const noexcept{
        auto* n = at(p);

        if(auto opt = n->asInt()){
            return static_cast<i16>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<i32> DOM::Value::get<i32>(StrView p) const noexcept{
        auto* n = at(p);

        if(auto opt = n->asInt()){
            return static_cast<i32>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<i64> DOM::Value::get<i64>(StrView p) const noexcept{
        auto* n = at(p);

        return n ? n->asInt() : std::nullopt;
    }

    template<>
    std::optional<u8> DOM::Value::get<u8>(StrView p) const noexcept{
        auto* n = at(p);

        if(auto opt = n->asInt()){
            return static_cast<u8>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<u16> DOM::Value::get<u16>(StrView p) const noexcept{
        auto* n = at(p);

        if(auto opt = n->asInt()){
            return static_cast<u16>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<u32> DOM::Value::get<u32>(StrView p) const noexcept{
        auto* n = at(p);

        if(auto opt = n->asInt()){
            return static_cast<u32>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<u64> DOM::Value::get<u64>(StrView p) const noexcept{
        auto* n = at(p);

        if(auto opt = n->asInt()){
            return static_cast<u64>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<f32> DOM::Value::get<f32>(StrView p) const noexcept{
        auto* n = at(p);

        if(auto opt = n->asFloat()){
            return static_cast<f32>(*opt);
        }

        return std::nullopt;
    }

    template<>
    std::optional<f64> DOM::Value::get<f64>(StrView p) const noexcept{
        auto* n = at(p);

        return n ? n->asFloat() : std::nullopt;
    }

    template<>
    std::optional<Str> DOM::Value::get<Str>(StrView p) const noexcept{
        auto* n = at(p);
        if(n == nullptr){
            return std::nullopt;
        }

        auto s = n->asString();
        if(s == nullptr){
            return std::nullopt;
        }
        return *s;
    }
}