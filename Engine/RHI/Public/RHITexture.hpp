#pragma once

#include "Semantics.hpp"
#include "Primitives.hpp"
#include "RHIDefinitions.hpp"

namespace Smol
{
    // GPU texture resource (1D, 2D, 3D, Cube, arrays)
    class RHITexture{
    public:
        SMOL_DECLARE_INTERFACE(RHITexture)

        virtual void Upload(
            const void* data,
            u32 mipLevel = 0,
            u32 arraySlice = 0
        ) = 0;

        virtual RHIPixelFormat GetFormat() const noexcept = 0;
        virtual u32 GetWidth() const noexcept = 0;
        virtual u32 GetHeight() const noexcept = 0;

        virtual void* GetNative() const noexcept = 0;

        virtual RHIResourceState GetState() const = 0;
        virtual void SetState(RHIResourceState state) = 0;
    };
}
