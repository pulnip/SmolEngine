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

        Vec2 spriteScale = Vec2{1, 1};

    public:
        SpriteComponent() = default;
        virtual ~SpriteComponent() = default;
        SMOL_DECLARE_PINNED(SpriteComponent)

        void OnAttach(StrView key);

        void Update(f32) override;

    private:
        void syncToRenderer();
    };
}
