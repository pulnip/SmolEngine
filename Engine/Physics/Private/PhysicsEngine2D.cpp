#include "Assert.hpp"
#include "Object.hpp"
#include "Shape2D.hpp"
#include "PhysicsEngine2D.hpp"

namespace Smol
{
    PhysicsEngine2D::PhysicsEngine2D(PhysicsEngine2D::Callback&& callback) noexcept
        : callback(std::move(callback))
    {
        SMOL_ASSERT(callback != nullptr);
    }

    PhysicsEngine2D::Handle PhysicsEngine2D::BindCollider(Collider2D collider){
        SMOL_ASSERT(collider.object != nullptr);
        SMOL_ASSERT(collider.object->IsA("ColliderComponent"));

        return colliders.push(collider);
    }

    void PhysicsEngine2D::Update(){
        SMOL_ASSERT(callback != nullptr);

        for(auto it = colliders.cbegin(); it != colliders.cend(); ++it){
            auto& A = *it;

            auto jt = it;
            ++jt;

            for(; jt != colliders.cend(); ++jt){
                auto& B = *jt;

                const auto filterA = (A.mask & B.layer) != 0;
                const auto filterB = (B.mask & A.layer) != 0;
                if(!filterA && !filterB)
                    continue;

                auto result = Collide(A.shape, B.shape, Vec2{5.0, 0});
                if(filterA){
                    callback(A.object, B.object, result);
                }
                if(filterB){
                    callback(B.object, A.object, result);
                }
            }
        }

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
