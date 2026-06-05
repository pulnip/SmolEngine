#pragma once

#include "Component.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"
#include "SpriteProxy.hpp"

namespace Smol
{
    class SpriteRenderer;

    class SpriteComponent: public TypedComponent<SpriteComponent>{
    private:
        SpriteProxy proxy;
        // TODO. use Resource Handle later
        RHITextureRAII sprite;

        const u32 frameCount = 8;
        const f32 framePerSeconds = 0.16f;

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

        void Update(f32) override;

    private:
        void syncToRenderer();
        void nextFrame() noexcept;
    };
}
