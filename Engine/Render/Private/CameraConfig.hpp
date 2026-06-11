#pragma once

#include "LinearAlgebra.hpp"
#include "Primitives.hpp"

namespace Smol
{
    // TODO. CameraComponent
    constexpr Smol::Vec3 camPos(0.0f, 0.0f, -1.0f);
    const auto camRot = Smol::unitQuat();
    const auto view = Smol::viewMat(camPos, camRot);

    constexpr auto screenWidth = 800.0f;
    constexpr auto screenHeight = 600.0f;
    constexpr auto aspect = screenWidth/screenHeight;

    constexpr auto fovY = static_cast<f32>(Smol::toRadian(50.0f));
    constexpr auto nearZ = 0.1f;
    constexpr auto farZ = 100.0f;

    // perspective projection template
    const auto ppProj = Smol::perspective(fovY, aspect, nearZ, farZ);
    // for perspective <-> orthographic convert
    constexpr auto focusDist = 30.0f;
    const auto tanHalfFovY = std::tan(0.5f * fovY);
    const auto halfH = focusDist * tanHalfFovY;
    const auto ogProj = Smol::orthographic(
        2.0f*halfH*aspect,
        2.0f*halfH,
        nearZ,
        farZ
    );

    // Screen height in World unit
    const auto viewHeight = 2 * halfH;
    const auto viewToScreen = screenHeight / viewHeight;
}
