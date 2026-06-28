#pragma once

#include <unordered_map>
#include "RHIAPI.hpp"
#include "RHIDefinitions.hpp"
#include "RHIBuffer.hpp"
#include "DX11Definitions.hpp"

namespace Smol
{
    class DX11Buffer: public RHIBuffer{
    private:
        BufferRAII buffer = nullptr;
        BufferRAII stagingBuffer = nullptr; // for CPURead download
        RHIResourceState currentState = RHIResourceState::Common;

        Device& device;
        DeviceContext& context;

        std::unordered_map<RHIBufferViewDesc, SRVRAII> srvs;
        std::unordered_map<RHIBufferViewDesc, UAVRAII> uavs;

    public:
        DX11Buffer(
            Device& device,
            DeviceContext& context,
            const RHIBufferCreateDesc& desc,
            StrView name
        );

        ~DX11Buffer();

        void Upload(
            const void* src,
            u32 srcSize,
            u32 offset
        ) RHI_OVERRIDE;

        void Download(
            void* dst,
            u32 dstSize,
            u32 offset
        ) RHI_OVERRIDE;

        u32 GetSize() const noexcept RHI_OVERRIDE;

        RHIResourceState GetState() const noexcept RHI_OVERRIDE{
            // NOTE. No-Op for DX11
            return currentState;
        }

        void SetState(RHIResourceState state) noexcept RHI_OVERRIDE{
            // NOTE. No-Op for DX11
            currentState = state;
        }

        Buffer* Get() noexcept{ return buffer.Get(); }
        const Buffer* Get() const noexcept{ return buffer.Get(); }

        SRV* GetOrCreateSRV(const RHIBufferViewDesc&);
        UAV* GetOrCreateUAV(const RHIBufferViewDesc&);
    };
}
