#include "Object.hpp"

namespace Smol
{
    auto Object::_SmolReflectImpl(){
        return ::Smol::Reflect<Object>()
            .SetName("Object")
            .Build();
    }
    namespace{
        const auto IsObjectRegistered = Object::_SmolReflectImpl();
    }
}
