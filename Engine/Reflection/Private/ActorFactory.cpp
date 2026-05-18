#include "ActorFactory.hpp"

namespace Smol
{
    ObjectRAII CreateObject(StrView name){
        return Smol::ClassRegistry::Create(name);
    }

    template <typename Base, typename Derived>
        requires std::is_base_of_v<Base, Derived>
    RAII<Derived> unique_cast(RAII<Base>& base){
        if(Derived* raw = dynamic_cast<Derived*>(base.get())){
            base.release();
            return RAII<Derived>(raw);
        }

        return nullptr;
    }

    ActorRAII CreateActor(StrView name){
        auto object = CreateObject(name);
        return unique_cast<Object, Actor>(object);
    }
}
