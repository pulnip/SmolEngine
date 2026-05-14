#include <unordered_map>
#include "Actor.hpp"
#include "ActorFactory.hpp"
#include "Primitives.hpp"

namespace Smol
{
    struct ActorFactory::Impl{
        std::unordered_map<Str, CreateFn> creaters;
    };

    ActorFactory::ActorFactory()
        : impl(std::make_unique<Impl>())
    {}

    ActorFactory::~ActorFactory(){}

    ActorFactory& ActorFactory::Get(){
        static ActorFactory factory;
        return factory;
    }

    bool ActorFactory::Register(Str name, CreateFn fn){
        auto& creaters = impl->creaters;
        auto [it, ret] = creaters.try_emplace(name, fn);

        return ret;
    }

    RAII<Actor> ActorFactory::Create(Str name){
        auto& creaters = impl->creaters;
        auto it = creaters.find(name);

        if(it == creaters.end()) return nullptr;

        return (it->second)();
    }
}
