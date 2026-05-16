#pragma once

#include <vector>
#include "ActorFWD.hpp"
#include "StringUtil.hpp"

namespace Smol
{
    class World{
    private:
        std::vector<ActorRAII> actors;
        StringHashMap<usize> indexByName;

    public:
        World(StrView scenePath);
        ~World();

        Actor* FindActorByName(StrView name) const;

        void Update(f32 deltaTime);
    };
}