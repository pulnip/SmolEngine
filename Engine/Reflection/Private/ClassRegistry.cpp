#include "Assert.hpp"
#include "Object.hpp"
#include "ClassRegistry.hpp"

namespace Smol
{
    ObjectRAII ClassRegistry::Create(StrView name, SpawnContext& context){
        static auto& registry = ClassRegistry::Get();
        static const auto& classByName = registry.classByName;

        auto it = classByName.find(name);
        if(it == classByName.end()){
            return nullptr;
        }

        const auto& desc = *it->second;
        return desc.factory(context);
    }

    ClassRegistry& ClassRegistry::Get(){
        static ClassRegistry singleton;
        return singleton;
    }

    bool ClassRegistry::Register(ClassDesc& desc){
        SMOL_ASSERT(!desc.name.empty());
        SMOL_ASSERT(desc.factory);
        auto [_, ret] = classByName.try_emplace(desc.name, &desc);

        return ret;
    }

    void ApplyProperties(const ClassDesc& desc, void* object, const DOM::Value& table){
        if(desc.parent != nullptr){
            ApplyProperties(*desc.parent, object, table);
        }

        for(const auto& [name, prop]: desc.properties){
            auto node = table.at(name);

            if(node == nullptr){
                // TODO. LOG WARN
                continue;
            }

            auto member = prop.accessor->Get(object);
            prop.typeInfo.deserialize(member, *node);
        }
    }

    ObjectRAII CreateObject(StrView name, SpawnContext& context){
        return ClassRegistry::Create(name, context);
    }
}
