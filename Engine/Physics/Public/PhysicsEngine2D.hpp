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
    class Object;
    class PhysicsEngine2D;

    class PhysicsProxy final{
    public:
        using Handle = GenericHandle<Collider2D>;

    private:
        Handle handle = Handle::InvalidHandle();
        PhysicsEngine2D* engine = nullptr;

    public:
        PhysicsProxy() noexcept = default;
        ~PhysicsProxy();
        SMOL_DECLARE_NON_COPYABLE(PhysicsProxy);
        PhysicsProxy(PhysicsProxy&&) noexcept;
        PhysicsProxy& operator=(PhysicsProxy&&) noexcept;

        PhysicsProxy(Handle handle, PhysicsEngine2D& engine)
            : handle(handle), engine(&engine){}

        bool IsValid() const noexcept{
            auto isEngineValid = engine != nullptr;
            auto isHandleValid = handle.IsValid();

            return isEngineValid && isHandleValid;
        }
        Collider2D& GetItem() noexcept;
    };

    class PhysicsEngine2D final{
    public:
        using Handle = PhysicsProxy::Handle;

    private:
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

        PhysicsProxy BindCollider(Collider2D);

        void Update();

        void MarkDestroy(Handle handle){
            pendingDestroy.push_back(handle);
        }

    private:
        std::vector<Handle> destroyScratch;

        void flushDestroy();

    private:
        friend class PhysicsProxy;

        Collider2D& GetItem(Handle);
        void Unbind(Handle);
    };
}
