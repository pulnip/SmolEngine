#pragma once

#include "CameraConfig.hpp"
#include "Primitives.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    struct RainStreakParam{
        f32 elapsedTime;
        f32 aspect = detail::aspect;
        f32 intensity = 0.5f;
        f32 speed = 1.0f;
        Vec3 color{0.7f, 0.8f, 1.0f};
        f32 slant = 0.15f;
    };

    struct RainDropletParam{
        f32 aspect = detail::aspect;
        f32 wetness = 0.05f;
        f32 strength = 2.0f;
        f32 density = 40.0f;
        Vec3 rimColor{0.9f, 0.95f, 1.0f};
        f32 specPower = 4.0f;
        Vec2 lightDir{-0.6f, -0.8f};
        f32 pad[2];
    };

    class PostRenderer final{
    private:
        RHIGraphicsPipelineStateRAII rainStreak;
        RHIBufferRAII rainStreakParam;
        u32 rainStreakParamSlot = 0;
        RHIGraphicsPipelineStateRAII rainDroplet;
        RHIBufferRAII rainDropletParam;
        u32 rainDropletParamSlot = 0;

    public:
        PostRenderer(RHIDevice&);
        ~PostRenderer();
        SMOL_DECLARE_PINNED(PostRenderer)

        void Upload(const RainStreakParam&);
        void Draw(RHICommandList&);
    };
}
