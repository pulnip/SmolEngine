#pragma once

#include "Component.hpp"
#include "GenericHandle.hpp"
#include "Resource.hpp"
#include "Semantics.hpp"
#include "SpriteProxy.hpp"

namespace Smol
{
    class Actor;
    class SpriteRenderer;

    class SpriteComponent: public TypedComponent<SpriteComponent>{
        SMOL_COMPONENT_BODY(SpriteComponent)

    private:
        using Handle = GenericHandle<SpriteResource>;
        Handle handle;

        SpriteProxy proxy;

    public:
        virtual ~SpriteComponent();
        SMOL_DECLARE_PINNED(SpriteComponent)

        SpriteComponent(const SpawnContext&);

        void Update(f32) override;
    };
}
