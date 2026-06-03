#pragma once

#include <span>
#include <vector>
#include "ActorFWD.hpp"
#include "StringUtil.hpp"
#include "Semantics.hpp"

namespace Smol
{
    struct SpawnContext;

    class World final{
    private:
        std::vector<ActorRAII> actors;
        StringHashMap<usize> indexByName;

    public:
        World();
        ~World();
        SMOL_DECLARE_MOVE_ONLY(World)

        World(StrView scenePath);
        // inject actors from outside
        World(SpawnContext&, std::span<const Str> actorNames);

        Actor* FindActorByName(StrView name) const;

        void Update(f32 deltaTime);
    };
}
