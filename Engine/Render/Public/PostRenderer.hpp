#pragma once

#include "CameraConfig.hpp"
#include "Primitives.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    struct RainCB{
        f32 elapsedTime;
        f32 aspect = detail::aspect;
        f32 intensity = 0.5f;
        f32 speed = 1.0f;
        Vec3 color{0.7f, 0.8f, 1.0f};
        f32 slant = 0.15f;
    };

    class PostRenderer final{
    private:
        RHIGraphicsPipelineStateRAII rainPipeline;
        RHISamplerRAII linearClamp;
        RHIBufferRAII rainCB;
        struct FragmentShaderSlot{
            static constexpr CStr rainCBSlot = "RainCB";
            u32 rainCB = 0;
            static constexpr CStr texSlot = "tex";
            u32 tex = 0;
            static constexpr CStr linearClampSlot = "linearClamp";
            u32 linearClamp = 0;
        } fs;

    public:
        PostRenderer(RHIDevice&);
        ~PostRenderer();
        SMOL_DECLARE_PINNED(PostRenderer)

        void Upload(const RainCB&);
        void Draw(RHICommandList&, RHITexture& scene);
    };
}
