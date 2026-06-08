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

    class SpriteAnimComponent: public TypedComponent<SpriteAnimComponent>{
        SMOL_COMPONENT_BODY(SpriteAnimComponent)

    private:
        using Handle = GenericHandle<SpriteResource>;
        Handle handle;
        u32 frameCount = 1;
        f32 framePerSeconds = 0.16f;

        SpriteProxy proxy;

        f32 elapsedTime = 0.0f;

        // if someone called setter of member variable below,
        // synced = false
        bool synced = true;
        u32 iframe = 0;

    public:
        SpriteAnimComponent() = default;
        virtual ~SpriteAnimComponent() = default;
        SMOL_DECLARE_PINNED(SpriteAnimComponent)

        void OnAttach(
            const DOM::Value&,
            const std::filesystem::path&
        );

        void Update(f32) override;

    private:
        void syncToRenderer();
        void nextFrame() noexcept;
    };
}
