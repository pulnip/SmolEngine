#pragma once

#include <functional>
#include "Collider2D.hpp"
#include "GenericHandle.hpp"
#include "Geometry/Overlap2D.hpp"
#include "Semantics.hpp"
#include "SlotMap.hpp"

namespace Smol
{
    class Object;

    class PhysicsEngine2D final{
        using Handle = GenericHandle<Collider2D>;

    private:
        SlotMap<Collider2D> colliders;
        using Callback = std::function<void(Object*, Object*, const SweepResult2D&)>;
        // World must setup this callback
        Callback callback;

        std::vector<Handle> pendingDestroy;

    public:
        ~PhysicsEngine2D() = default;
        SMOL_DECLARE_PINNED(PhysicsEngine2D)

        PhysicsEngine2D(Callback&&) noexcept;

        Handle BindCollider(Collider2D);

        void Update();

        void MarkDestroy(Handle handle){
            pendingDestroy.push_back(handle);
        }

    private:
        std::vector<Handle> destroyScratch;

        void flushDestroy();
    };
}
