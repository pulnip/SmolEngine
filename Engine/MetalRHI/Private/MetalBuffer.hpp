#pragma once

#include <Metal/MTLDevice.hpp>
#include <Metal/MTLBuffer.hpp>
#include "RHIAPI.hpp"
#include "RHIDefinitions.hpp"
#include "RHIBuffer.hpp"

namespace Smol
{
    class MetalBuffer final: public RHIBuffer{
    private:
        MTL::Buffer* buffer;
        usize size = 0;
        RHIBufferUsage usage = RHIBufferUsage::None;
        bool isManaged = false;
        RHIResourceState currentState = RHIResourceState::Common;

    public:
        MetalBuffer(
            MTL::Device& device,
            const RHIBufferCreateDesc& desc,
            StrView name = {}
        );
        ~MetalBuffer();

        void Upload(
            const void* data,
            u32 size,
            u32 offset = 0
        ) RHI_OVERRIDE;

        void Download(
            void* data,
            u32 size,
            u32 offset = 0
        ) RHI_OVERRIDE;

        u32 GetSize() const noexcept RHI_OVERRIDE{
            return size;
        }

        MTL::Buffer* Get() const noexcept{ return buffer; }

        RHIResourceState GetState() const RHI_OVERRIDE{
            return currentState;
        }

        void SetState(RHIResourceState state) RHI_OVERRIDE{
            currentState = state;
        }
    };
}
