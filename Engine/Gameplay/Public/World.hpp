#pragma once

#include <unordered_map>
#include <vector>
#include "ActorFWD.hpp"
#include "GenericHandle.hpp"
#include "Semantics.hpp"
#include "SlotMap.hpp"
#include "StringUtil.hpp"

namespace Smol
{
    struct SpawnContext;

    class World final{
    private:
        using Handle = GenericHandle<ActorRAII>;
        SlotMap<ActorRAII> actors;
        StringHashMap<Handle> handleMap;
        std::unordered_map<Handle, Str> handleToName;

        std::vector<Handle> pendingDestory;

        bool isShutdown = false;

    public:
        World();
        ~World();
        SMOL_DECLARE_MOVE_ONLY(World)

        World(StrView scenePath);
        // inject actors from outside
        World(SpawnContext&);

        Actor* FindActorByName(StrView name) const;

        void Update(f32 deltaTime);

        void MarkDestroy(Handle);

        bool IsShutdown() const noexcept{ return isShutdown; }

    private:
        std::vector<Handle> destroyScratch;

        void flushDestroy();
    };
}
