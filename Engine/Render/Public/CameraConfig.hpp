#pragma once

#include "LinearAlgebra.hpp"
#include "Primitives.hpp"

namespace Smol
{
    namespace detail{
        // TODO. CameraComponent
        constexpr Vec3 camPos(0.0f, 0.0f, -1.0f);
        const auto camRot = unitQuat();
        const auto view = viewMat(camPos, camRot);

        constexpr auto screenWidth = 800.0f;
        constexpr auto screenHeight = 600.0f;
        constexpr auto aspect = screenWidth/screenHeight;

        constexpr auto fovY = static_cast<f32>(toRadian(50.0f));
        constexpr auto nearZ = 0.1f;
        constexpr auto farZ = 100.0f;

        // perspective projection template
        const auto ppProj = perspective(fovY, aspect, nearZ, farZ);
        // for perspective <-> orthographic convert
        constexpr auto focusDist = 30.0f;
        const auto tanHalfFovY = std::tan(0.5f * fovY);
        const auto halfH = focusDist * tanHalfFovY;
        const auto ogProj = orthographic(
            2.0f*halfH*aspect,
            2.0f*halfH,
            nearZ,
            farZ
        );

        // Screen height in World unit
        const auto viewHeight = 2 * halfH;
        const auto viewToScreen = screenHeight / viewHeight;
    }

    struct CameraView{
        Mat4 view = detail::view;
        f32 viewToScreen = detail::viewToScreen;
        f32 screenWidth = detail::screenWidth, screenHeight = detail::screenHeight;
        // z pos of view plane.
        // used at perspective <-> orthographic convert
        f32 focusDist = 30.0f;
    };

    // Expect Orthographic
    inline Vec2 WorldToScreen(Vec3 world, CameraView cam = {}) noexcept{
        const auto viewPos = cam.view * toVec4(world, 1.0f);

        return Vec2{
            0.5f * cam.screenWidth + viewPos.x * cam.viewToScreen,
            0.5f * cam.screenHeight - viewPos.y * cam.viewToScreen,
        };
    }

    inline constexpr Vec3 ScreenToWorld(Vec2 screen, CameraView cam = {}) noexcept{
        const Vec3 viewPos{
            (screen.x - 0.5f * cam.screenWidth) / cam.viewToScreen,
            (0.5f * cam.screenHeight - screen.y) / cam.viewToScreen,
            cam.focusDist
        };
        const auto invView = inverseRigid(cam.view);

        return static_cast<Vec3>(invView * toVec4(viewPos, 1.0f));
    }

    inline constexpr Vec2 ScreenToWorld2D(Vec2 screen, CameraView cam = {}) noexcept{
        const Vec3 viewPos{
            (screen.x - 0.5f * cam.screenWidth) / cam.viewToScreen,
            (0.5f * cam.screenHeight - screen.y) / cam.viewToScreen,
            cam.focusDist
        };
        const auto invView = inverseRigid(cam.view);

        return static_cast<Vec2>(invView * toVec4(viewPos, 1.0f));
    }
}
