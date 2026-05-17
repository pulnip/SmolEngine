#pragma once

#include "Semantics.hpp"
#include "RHIDefinitions.hpp"

namespace Smol
{
    class RHIBuffer{
    public:
        SMOL_DECLARE_INTERFACE(RHIBuffer)

        virtual void Upload(
            const void* data, u32 size,
            u32 offset = 0
        ) = 0;

        // Notice! only valid for Metal and D3D11
        virtual void Download(
            void* data, u32 size,
            u32 offset = 0
        ) = 0;

        virtual u32 GetSize() const noexcept = 0;

        virtual RHIResourceState GetState() const = 0;
        virtual void SetState(RHIResourceState state) = 0;
    };
}
