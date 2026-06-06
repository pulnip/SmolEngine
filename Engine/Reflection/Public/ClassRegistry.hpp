#pragma once

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
    struct SpawnContext;

    template<typename T>
    concept SpawnConstructible = requires(::Smol::SpawnContext ctx){
        { T(ctx) };
    };

    // Reflection target is Object
    class Object;
    using ObjectRAII = RAII<Object>;

    struct ClassDesc{
        Str name;
        const ClassDesc* parent = nullptr;
        std::function<ObjectRAII(const SpawnContext&)> factory;
        StringHashMap<PropertyDesc> properties;

        template<typename Class, typename Member>
        PropertyDesc& AddProperty(CStr name, Member Class::* member){
            auto [it, ret] = properties.try_emplace(
                name,
                PropertyDesc{
                    .typeInfo = *GetTypeInfo<Member>(),
                    .accessor = std::make_unique<MemberAccessor<Class, Member>>(member),
                    .meta = {}
                }
            );

            SMOL_ASSERT(ret);
            return it->second;
        }

        template<typename Class, typename Member, typename Leaf>
        PropertyDesc& AddProperty(CStr name, Member Class::* member, Leaf Member::* leaf){
            auto [it, ret] = properties.try_emplace(
                name,
                PropertyDesc{
                    .typeInfo = *GetTypeInfo<Leaf>(),
                    .accessor = std::make_unique<NestedMemberAccessor<Class, Member, Leaf>>(member, leaf),
                    .meta = {}
                }
            );

            SMOL_ASSERT(ret);
            return it->second;
        }
    };

    class ClassRegistry{
    private:
        std::unordered_map<std::type_index, RAII<ClassDesc>> classByTypeindex;
        StringHashMap<ClassDesc*> classByName;

    public:
        static ObjectRAII Create(StrView type, const SpawnContext&);

    private:
        template<typename T>
            requires std::is_base_of_v<Object, T>
        friend class ClassBuilder;

    public:
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

    template<typename T>
        requires std::is_base_of_v<Object, T>
    class ClassBuilder;

    template<typename T>
        requires std::is_base_of_v<Object, T>
    ClassBuilder<T> Reflect();

    template<typename T>
        requires std::is_base_of_v<Object, T>
    class ClassBuilder{
        ClassDesc& desc;
        PropertyDesc* lastProp = nullptr;

    public:
        ClassBuilder& SetName(CStr name){
            desc.name = name;

            return *this;
        }

        template<typename Parent>
        ClassBuilder& Inherits(){
            desc.parent = &ClassRegistry::Get().DescFor<Parent>();

            return *this;
        }

        ClassBuilder& SetFactory(std::function<RAII<T>(const SpawnContext&)>&& f){
            desc.factory = [f = std::move(f)](const SpawnContext& context) -> RAII<Object> {
                return f(context);
            };

            return *this;
        }

        template<typename Member>
        ClassBuilder& SetProperty(CStr name, Member T::* member){
            lastProp = &desc.AddProperty(name, member);

            return *this;
        }

        template<typename Member, typename Leaf>
        ClassBuilder& SetProperty(CStr name, Member T::* member, Leaf Member::*leaf){
            lastProp = &desc.AddProperty(name, member, leaf);

            return *this;
        }

        ClassBuilder& SetTooltip(CStr tooltip){
            if(lastProp != nullptr){
                lastProp->meta.tooltip = tooltip;
            }

            return *this;
        }

        bool Build(){
            auto& registry = ClassRegistry::Get();
            return registry.Register(desc);
        }

    private:
        friend ClassBuilder Reflect<T>();

        ClassBuilder()
            : desc(ClassRegistry::Get().DescFor<T>())
        {
            if constexpr (SpawnConstructible<T>){
                desc.factory = [](const SpawnContext& context) -> ObjectRAII {
                    return std::make_unique<T>(context);
                };
            }
            else if constexpr (std::is_default_constructible_v<T>){
                desc.factory = [](const SpawnContext&) -> ObjectRAII {
                    return std::make_unique<T>();
                };
            }
        }
    };

    ObjectRAII CreateObject(StrView type, const SpawnContext& context);
}
