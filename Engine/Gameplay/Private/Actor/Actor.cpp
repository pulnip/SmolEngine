#include "Actor.hpp"
#include "Rigidbody.hpp"

namespace Smol
{
    template<>
    RigidBody* Actor::AddComponent<RigidBody>(){
        constexpr auto index = GetComponentTypeIndex<RigidBody>();
        auto c = std::make_unique<RigidBody>();
        builtinComponents[index] = std::move(c);

        return static_cast<RigidBody*>(builtinComponents[index].get());
    }

    template<>
    RigidBody* Actor::GetComponent<RigidBody>(){
        constexpr auto index = GetComponentTypeIndex<RigidBody>();
        return static_cast<RigidBody*>(builtinComponents[index].get());
    }
}
