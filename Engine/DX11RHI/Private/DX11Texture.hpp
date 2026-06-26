#pragma once

#include <unordered_map>
#include "DX11Definitions.hpp"
#include "RHIAPI.hpp"
#include "RHIDefinitions.hpp"
#include "RHITexture.hpp"

namespace Smol
{
    class DX11Texture final: public RHITexture{
    private:
        TextureRAII texture = nullptr;
        RHIResourceState currentState = RHIResourceState::Common;

        Device& device;

        std::unordered_map<RHITextureViewDesc, SRVRAII> srvs;
        std::unordered_map<RHITextureViewDesc, RTVRAII> rtvs;
        std::unordered_map<RHITextureViewDesc, UAVRAII> uavs;
        std::unordered_map<RHITextureViewDesc, DSVRAII> dsvs;

    public:
        DX11Texture(
            Device& device,
            const RHITextureCreateDesc& desc,
            StrView name
        );
        DX11Texture(
            Device& device,
            Swapchain& swapchain,
            StrView name = {}
        );

        ~DX11Texture();

        void Upload(
            const void* data,
            u32 mipLevel,
            u32 arraySlice
        ) RHI_OVERRIDE;

        RHIPixelFormat GetFormat() const noexcept RHI_OVERRIDE;
        u32 GetWidth() const noexcept RHI_OVERRIDE;
        u32 GetHeight() const noexcept RHI_OVERRIDE;

        void* GetNative() const noexcept RHI_OVERRIDE{
            return texture.Get();
        }

        RHIResourceState GetState() const RHI_OVERRIDE{
            return currentState;
        }

        void SetState(RHIResourceState state) RHI_OVERRIDE{
            currentState = state;
        }

        Texture* Get() const{ return texture.Get(); }

        SRV* GetOrCreateSRV(const RHITextureViewDesc&);
        RTV* GetOrCreateRTV(const RHITextureViewDesc&);
        UAV* GetOrCreateUAV(const RHITextureViewDesc&);
        DSV* GetOrCreateDSV(const RHITextureViewDesc&);

        SRV* GetOrCreateSRV();
        RTV* GetOrCreateRTV();
        UAV* GetOrCreateUAV();
        DSV* GetOrCreateDSV();
    };
}
