#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
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
        std::vector<PropertyDesc> properties;

        template<typename Class, typename Member>
        PropertyDesc& AddProperty(Str name, Member Class::* member){
            return properties.emplace_back(PropertyDesc{
                .name = std::move(name),
                .typeInfo = GetTypeInfo<Member>(),
                .accessor = std::make_unique<MemberAccessor<Class, Member>>(member),
                .meta = {}
            });
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
}
