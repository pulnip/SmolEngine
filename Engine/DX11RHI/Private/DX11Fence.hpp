#pragma once

#include "RHIAPI.hpp"
#include "RHIFence.hpp"
#include "DX11Definitions.hpp"

namespace Smol
{
    class DX11Fence: public RHIFence{
    public:
        DX11Fence(
            Device& device,
            u64 initialValue
        ){}

        void WaitCPU(u64 waitValue, u64 timeoutMs) noexcept RHI_OVERRIDE{
            // NOTE. No-Op for DX11
        }

        u64 GetValue() noexcept RHI_OVERRIDE{
            // NOTE. No-Op for DX11
            return 0;
        }

        bool IsComplete(u64 value) noexcept RHI_OVERRIDE{
            // NOTE. No-Op for DX11
            return true;
        }
    };
}
