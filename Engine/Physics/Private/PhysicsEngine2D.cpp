#include <vector>
#include "Assert.hpp"
#include "Collider2D.hpp"
#include "Geometry/Overlap2D.hpp"
#include "Object.hpp"
#include "Shape2D.hpp"
#include "PhysicsEngine2D.hpp"

namespace Smol
{
    PhysicsProxy::PhysicsProxy(PhysicsProxy&& other) noexcept
        : handle(other.handle)
        , engine(other.engine)
    {
        other.handle = Handle::InvalidHandle();
        other.engine = nullptr;
    }
    PhysicsProxy& PhysicsProxy::operator=(PhysicsProxy&& other) noexcept{
        handle = other.handle;
        engine = other.engine;

        other.handle = Handle::InvalidHandle();
        other.engine = nullptr;

        return *this;
    }

    PhysicsProxy::~PhysicsProxy(){
        if(IsValid()){
            engine->Unbind(handle);

            handle = Handle::InvalidHandle();
            engine = nullptr;
        }
    }

    Collider2D& PhysicsProxy::GetRef() noexcept{
        SMOL_ASSERT(engine != nullptr);
        return engine->GetRef(handle);
    }

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

    PhysicsProxy PhysicsEngine2D::BindCollider(Collider2D collider){
        SMOL_ASSERT(collider.object != nullptr);
        SMOL_ASSERT(collider.object->IsA("ColliderComponent"));

        auto handle = colliders.Push(collider);
        return PhysicsProxy(handle, *this);
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

                const auto filterA = (A.mask & B.layer) != 0;
                const auto filterB = (B.mask & A.layer) != 0;

                if(!filterA && !filterB)
                    continue;

                const auto result = Collide(A.shape, B.shape);
                if(!result.overlapping)
                    continue;

                auto key = MakeKey(handleA, handleB);

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

        // fire OnExit for difference within previous frame
        for(const auto& key: prev){
            if(curr.count(key))
                continue;

            auto a = colliders.Find(key.a);
            auto b = colliders.Find(key.b);
            SMOL_ASSERT(a != nullptr && b != nullptr);

            const auto filterA = (a->mask & b->layer) != 0;
            const auto filterB = (b->mask & a->layer) != 0;

            if(filterA)
                onExit(a->object, b->object);
            if(filterB)
                onExit(b->object, a->object);
        }

        prev = std::move(curr);
        FlushDestroy();
    }


    void PhysicsEngine2D::FlushDestroy(){
        while(!pendingDestroy.empty()){
            std::swap(pendingDestroy, destroyScratch);

            for(const auto& handle: destroyScratch){
                for(auto it = prev.begin(); it != prev.end();){
                    if(it->a != handle && it->b != handle){
                        ++it;
                        continue;
                    }

                    auto a = colliders.Find(it->a);
                    auto b = colliders.Find(it->b);
                    SMOL_ASSERT(a != nullptr && b != nullptr);

                    const auto filterA = (a->mask & b->layer) != 0;
                    const auto filterB = (b->mask & a->layer) != 0;

                    if(filterA)
                        onExit(a->object, b->object);
                    if(filterB)
                        onExit(b->object, a->object);

                    it = prev.erase(it);
                }
            }

            destroyScratch.clear();
        }
    }

    Collider2D& PhysicsEngine2D::GetRef(Handle handle){
        return colliders.GetRef(handle);
    }

    void PhysicsEngine2D::Unbind(Handle handle){
        colliders.Remove(handle);
    }
}
