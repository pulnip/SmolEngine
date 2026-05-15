#pragma once

#include <format>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include "Assert.hpp"
#include "Primitives.hpp"
#include "PropertyDesc.hpp"
#include "StringUtil.hpp"
#include "TypeInfo.hpp"

namespace Smol
{
    struct ClassDesc{
        Str name;
        const ClassDesc* parent = nullptr;
        std::function<ObjectRAII()> factory;
        StringHashMap<PropertyDesc> properties;

        template<typename Class, typename Member>
        PropertyDesc& AddProperty(Str name, Member Class::* member){
            auto topoName = std::format("{}.{}", this->name, name);

            auto [it, ret] = properties.try_emplace(std::move(topoName), PropertyDesc{
                .typeInfo = *GetTypeInfo<Member>(),
                .accessor = std::make_unique<MemberAccessor<Class, Member>>(member),
                .meta = {}
            });

            SMOL_ASSERT(ret);
            return it->second;
        }
    };

    class ClassRegistry{
    private:
        std::unordered_map<std::type_index, RAII<ClassDesc>> classByTypeindex;
        StringHashMap<ClassDesc*> classByName;

    public:
        static ObjectRAII Create(StrView name);

    private:
        template<typename T>
            requires std::is_base_of_v<Object, T>
        friend class ClassBuilder;

        static ClassRegistry& Get();

        template<typename T>
        ClassDesc& DescFor(){
            auto& slot = classByTypeindex[std::type_index(typeid(T))];

            if(slot == nullptr){
                slot = std::make_unique<ClassDesc>();
            }

            return *slot;
        }

        bool Register(ClassDesc& desc);
    };

    void ApplyProperties(const ClassDesc&, void* object, const DOM::Value&);
}
