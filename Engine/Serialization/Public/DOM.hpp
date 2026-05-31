#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>
#include "CoreFWD.hpp"
#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    namespace DOM
    {
        class Value;

        using Array = std::vector<Value>;
        using Table = std::unordered_map<Str, Value>;

        class Value{
        private:
            using ArrayRAII = RAII<Array>;
            using TableRAII = RAII<Table>;

            std::variant<
                std::monostate,
                bool,
                i64,
                f64,
                Str,
                ArrayRAII,
                TableRAII
            > value = std::monostate();

        public:
            Value() = default;
            Value(bool v) noexcept: value(v){}
            Value(i64 v) noexcept: value(v){}
            Value(f64 v) noexcept: value(v){}
            Value(Str v):value(v){}
            Value(const char* v):value(Str(v)){}
            Value(Array&& v): value(std::make_unique<Array>(std::move(v))){}
            Value(Table&& v): value(std::make_unique<Table>(std::move(v))){}

            SMOL_DECLARE_MOVE_ONLY_NOEXCEPT(Value)

            enum class Kind{
                None,
                Bool,
                Int,
                Float,
                String,
                Array,
                Table
            };

            Kind kind() const noexcept;

            bool is_none() const noexcept{ return std::holds_alternative<std::monostate>(value); }
            bool is_bool() const noexcept{ return std::holds_alternative<bool>(value); }
            bool is_int() const noexcept{ return std::holds_alternative<i64>(value); }
            bool is_float() const noexcept{ return std::holds_alternative<f64>(value); }
            bool is_string() const noexcept{ return std::holds_alternative<Str>(value); }
            bool is_array() const noexcept{ return std::holds_alternative<ArrayRAII>(value); }
            bool is_table() const noexcept{ return std::holds_alternative<TableRAII>(value); }

            std::optional<bool> asBool() const noexcept;
            // expect like 1, and 1.0 cannot be Integer
            std::optional<i64> asInt() const noexcept;
            // expect like 1.0, also 1 can be Float
            std::optional<f64> asFloat() const noexcept;
            const Str* asString() const noexcept;
            const Array* asArray() const noexcept;
            const Table* asTable() const noexcept;

            const Value* at(StrView path) const noexcept;

            template<typename T>
            std::optional<T> get(StrView path) const noexcept;

            template<typename F>
            void forEach(StrView p, F&& fn) const{
                auto n = at(p);
                if(n == nullptr) return;

                auto arr = n->asArray();
                if(arr == nullptr) return;

                for(auto& val: *arr){
                    fn(val);
                }
            }
        };
    }
}
