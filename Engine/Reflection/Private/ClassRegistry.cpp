#include "Assert.hpp"
#include "Object.hpp"
#include "ClassRegistry.hpp"

namespace Smol
{
    ObjectRAII ClassRegistry::Create(StrView name){
        static auto& registry = ClassRegistry::Get();
        static const auto& classByName = registry.classByName;

        auto it = classByName.find(name);
        if(it == classByName.end()){
            return nullptr;
        }

        const auto& desc = *it->second;
        return desc.factory();
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
}
