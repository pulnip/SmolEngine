#pragma once

#include "ActorFactory.hpp"

namespace Smol
{
    class Actor{
    public:
        Actor() = default;
        virtual ~Actor() = default;

        virtual void update(float) = 0;
    };
}

#define SMOL_ACTOR(Type) \
namespace{ \
    const auto _##Type##_registered = []{ \
        return Smol::ActorFactory::Get().Register(#Type, []{ \
            return std::make_unique<Type>(); \
        }); \
    }(); \
}
