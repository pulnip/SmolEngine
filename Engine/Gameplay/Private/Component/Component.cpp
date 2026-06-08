#include "Assert.hpp"
#include "Component.hpp"

namespace Smol
{
    void Component::MarkManaged(Actor* owner) noexcept{
        SMOL_ASSERT(this->owner == nullptr);
        this->owner = owner;
    }
}
