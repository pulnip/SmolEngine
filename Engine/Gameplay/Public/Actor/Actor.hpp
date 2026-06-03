#pragma once

#include <array>
#include <unordered_map>
#include <vector>
#include "ActorFWD.hpp"
#include "Component.hpp"
#include "Primitives.hpp"
#include "Object.hpp"

namespace Smol
{
    class Actor: public Object{
    private:
        std::array<ComponentRAII, NUM_BUILTIN_COMPONENTS> builtinComponents;
        std::unordered_map<Component::TypeID, ComponentRAII> userdefinedComponents;

        std::vector<ActorRAII> children;

    protected:
        Transform transform;

    public:
        Actor() = default;
        virtual ~Actor() = default;
        SMOL_DECLARE_MOVE_ONLY(Actor)

        virtual void OnUpdate(f32){}

        virtual void SyncRenderState(){}

        template<typename T, class... Args>
            requires (!IsBuiltinComponent<T>())
        T* AddComponent(Args&&... args){
            constexpr auto typeID = TypedComponent<T>::GetStaticTypeID();
            auto c = std::make_unique<T>(std::forward<Args>(args)...);

            auto [it, ret] = userdefinedComponents.try_emplace(typeID, std::move(c));
            if(!ret){
                return nullptr;
            }

            return static_cast<T*>(it->second.get());
        }
        template<typename T, class... Args>
            requires (IsBuiltinComponent<T>())
        T* AddComponent(Args&&...);

        template<typename T>
            requires (!IsBuiltinComponent<T>())
        T* GetComponent(){
            constexpr auto typeID = TypedComponent<T>::GetStaticTypeID();

            // TODO. use unordered_map later
            auto it = userdefinedComponents.find(typeID);
            if(it == userdefinedComponents.end()){
                return nullptr;
            }

            return static_cast<T*>(it->second.get());
        }
        template<typename T>
            requires (IsBuiltinComponent<T>())
        T* GetComponent();
    };
}
