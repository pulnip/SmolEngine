#pragma once

#include <functional>
#include <unordered_set>
#include "Collider2D.hpp"
#include "GenericHandle.hpp"
#include "HashUtil.hpp"
#include "Geometry/Overlap2D.hpp"
#include "Semantics.hpp"
#include "SlotMap.hpp"

namespace Smol
{
    enum class ContactPhase{
        Enter,
        Stay,
        Exit
    };

    class Object;

    class PhysicsEngine2D final{
        using Handle = GenericHandle<Collider2D>;

        struct PairKey{
            Handle a, b;

            bool operator==(const PairKey& other) const noexcept{
                return a == other.a && b == other.b;
            }
        };
        static PairKey MakeKey(Handle a, Handle b){
            return a.GetIndex() < b.GetIndex() ?
                PairKey{a, b} :
                PairKey{b, a};
        }
        struct PairKeyHash{
            usize operator()(const PairKey& key) const noexcept{
                return hashAll(key.a, key.b);
            }
        };

    private:
        SlotMap<Collider2D> colliders;
        using OnEnterCallback = std::function<void(Object*, Object*, const OverlapResult2D&)>;
        using OnStayCallback = OnEnterCallback;
        using OnExitCallback = std::function<void(Object*, Object*)>;
        // World must setup this callback
        OnEnterCallback onEnter;
        OnStayCallback onStay;
        OnExitCallback onExit;

        using ContactSet = std::unordered_set<PairKey, PairKeyHash>;
        ContactSet prev;

        std::vector<Handle> pendingDestroy;

    public:
        ~PhysicsEngine2D() = default;
        SMOL_DECLARE_PINNED(PhysicsEngine2D)

        PhysicsEngine2D(OnEnterCallback&&, OnStayCallback&&, OnExitCallback&&) noexcept;

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
