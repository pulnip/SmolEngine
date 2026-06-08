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
        SpriteComponent() = default;
        virtual ~SpriteComponent() = default;
        SMOL_DECLARE_PINNED(SpriteComponent)

        void OnAttach(
            const DOM::Value&,
            const std::filesystem::path&
        );

        void Update(f32) override;
    };
}
