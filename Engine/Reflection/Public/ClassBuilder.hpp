#pragma once

#include <type_traits>
#include "ClassRegistry.hpp"
#include "CoreFWD.hpp"
#include "PropertyDesc.hpp"

namespace Smol
{
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

        ClassBuilder& SetFactory(std::function<RAII<T>()> f){
            desc.factory = [f = std::move(f)]() -> RAII<Object> {
                return f();
            };

            return *this;
        }

        template<typename Member>
        ClassBuilder& SetProperty(CStr name, Member T::* member){
            lastProp = &desc.AddProperty(name, member);

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
            if constexpr (std::is_default_constructible_v<T>){
                desc.factory = []() -> ObjectRAII {
                    return std::make_unique<T>();
                };
            }
        }
    };

    template<typename T>
        requires std::is_base_of_v<Object, T>
    ClassBuilder<T> Reflect(){
        return ClassBuilder<T>();
    }
}
