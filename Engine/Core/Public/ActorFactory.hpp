#pragma once

#include <functional>
#include "ActorFWD.hpp"
#include "Semantics.hpp"
#include "Primitives.hpp"

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

        bool Register(Str name, CreateFn);
        RAII<Actor> Create(Str name);
    };
}
