#pragma once

#include <functional>
#include <string>
#include "CoreFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class ActorFactory final{
    private:
        using CreateFn = std::function<ActorRAII()>;

        struct Impl;
        RAII<Impl> impl;

    public:
        ActorFactory();
        ~ActorFactory();
        SMOL_DECLARE_PINNED(ActorFactory)

        static ActorFactory& Get();

        bool Register(std::string name, CreateFn);
        RAII<Actor> Create(std::string name);
    };
}
