#include <vector>
#include "Assert.hpp"
#include "Geometry/Overlap2D.hpp"
#include "Object.hpp"
#include "Shape2D.hpp"
#include "PhysicsEngine2D.hpp"

namespace Smol
{
    PhysicsEngine2D::PhysicsEngine2D(
        OnEnterCallback&& onEnter,
        OnStayCallback&& onStay,
        OnExitCallback&& onExit
    ) noexcept
        : onEnter(std::move(onEnter))
        , onStay(std::move(onStay))
        , onExit(std::move(onExit))
    {
        SMOL_ASSERT(this->onEnter != nullptr);
        SMOL_ASSERT(this->onStay != nullptr);
        SMOL_ASSERT(this->onExit != nullptr);
    }

    PhysicsEngine2D::Handle PhysicsEngine2D::BindCollider(Collider2D collider){
        SMOL_ASSERT(collider.object != nullptr);
        SMOL_ASSERT(collider.object->IsA("ColliderComponent"));

        return colliders.push(collider);
    }

    void PhysicsEngine2D::Update(){
        ContactSet curr;
        struct Hit{
            Object *a, *b;
            OverlapResult2D result;
            bool filterA, filterB;
            PairKey key;
        };
        std::vector<Hit> hits;

        // Collect Overlap Pair
        for(auto it = colliders.cbegin(); it != colliders.cend(); ++it){
            auto& A = *it;
            auto handleA = it.MakeHandle();

            auto jt = it;
            ++jt;

            for(; jt != colliders.cend(); ++jt){
                auto& B = *jt;
                auto handleB = jt.MakeHandle();

                const auto result = Collide(A.shape, B.shape);
                if(!result.overlapping)
                    continue;

                auto key = MakeKey(handleA, handleB);
                const auto filterA = (A.mask & B.layer) != 0;
                const auto filterB = (B.mask & A.layer) != 0;

                curr.insert(key);
                hits.push_back({
                    .a = A.object, .b = B.object,
                    .result = result,
                    .filterA = filterA,
                    .filterB = filterB,
                    .key = key
                });
            }
        }

        // Fire Event
        for(const auto& hit: hits){
            const auto isEnter = !prev.count(hit.key);
            const auto& callback = isEnter ? onEnter : onStay;

            if(hit.filterA)
                callback(hit.a, hit.b, hit.result);
            if(hit.filterB)
                callback(hit.b, hit.a, hit.result);
        }
        for(const auto& key: prev){
            if(curr.count(key))
                continue;

            auto a = colliders.get(key.a).object;
            auto b = colliders.get(key.b).object;
            onExit(a, b);
        }

        prev = std::move(curr);
        flushDestroy();
    }

    void PhysicsEngine2D::flushDestroy(){
        std::swap(pendingDestroy, destroyScratch);

        for(auto& handle: destroyScratch){
            colliders.remove(handle);
        }

        destroyScratch.clear();
    }
}
