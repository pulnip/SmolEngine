#pragma once

#include "ComponentFWD.hpp"
#include "Object.hpp"

namespace Smol
{
    class Component: public Object{
    public:
        using TypeID = const void*;

    public:
        SMOL_DECLARE_INTERFACE(Component)

        virtual TypeID GetTypeID() const = 0;
    };

    template<typename Derived>
    class TypedComponent: public Component{
    private:
        inline static u8 typeIDStorage = 0;

    public:
        static constexpr TypeID GetStaticTypeID(){ return &typeIDStorage; }
        TypeID GetTypeID() const override final{ return &typeIDStorage; }
    };

    class RigidBody: public TypedComponent<RigidBody>{

    };
}