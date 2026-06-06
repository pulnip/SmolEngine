#pragma once

#include "Component.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"
#include "SpriteProxy.hpp"

namespace Smol
{
    class SpriteRenderer;

    struct SpriteResource{
        const RHITextureRAII texture;

        const u32 frameCount = 1;
        const f32 framePerSeconds = 0.16f;
    };

    class SpriteComponent: public TypedComponent<SpriteComponent>{
        SMOL_COMPONENT_BODY(SpriteComponent)

    private:
        // TODO. use Resource Handle later
        SpriteResource resource;

        SpriteProxy proxy;

        f32 elapsedTime = 0.0f;

        // if someone called setter of member variable below,
        // synced = false
        bool synced = true;
        Transform& transform;
        u32 iframe = 0;

    public:
        virtual ~SpriteComponent();
        SMOL_DECLARE_PINNED(SpriteComponent)

        SpriteComponent(RHITextureRAII&&, Transform&, SpriteRenderer&);
        SpriteComponent(const SpawnContext&);

        void Update(f32) override;

    private:
        void syncToRenderer();
        void nextFrame() noexcept;
    };
}
