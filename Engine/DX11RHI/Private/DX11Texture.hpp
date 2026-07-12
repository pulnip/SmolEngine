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

        Device& device;

        std::unordered_map<RHITextureViewDesc, SRVRAII> srvs;
        std::unordered_map<RHITextureViewDesc, RTVRAII> rtvs;
        std::unordered_map<RHITextureViewDesc, UAVRAII> uavs;
        std::unordered_map<RHITextureViewDesc, DSVRAII> dsvs;

    public:
        DX11Texture(
            Device& device,
            const RHITextureCreateDesc& desc,
            StrView name = {}
        );
        DX11Texture(
            Device& device,
            Swapchain& swapchain,
            StrView name = {}
        );

        ~DX11Texture();

        RHIPixelFormat GetFormat() const noexcept RHI_OVERRIDE;
        u32 GetWidth() const noexcept RHI_OVERRIDE;
        u32 GetHeight() const noexcept RHI_OVERRIDE;

        void* GetNative() noexcept RHI_OVERRIDE{
            return texture.Get();
        }

        Texture* Get() noexcept{ return texture.Get(); }

        SRV* GetOrCreateSRV(const RHITextureViewDesc&);
        RTV* GetOrCreateRTV(const RHITextureViewDesc&);
        UAV* GetOrCreateUAV(const RHITextureViewDesc&);
        DSV* GetOrCreateDSV(const RHITextureViewDesc&);

        SRV* GetOrCreateSRV(){
            return GetOrCreateSRV(RHITextureViewDesc{
                .format = GetFormat()
            });
        }
        RTV* GetOrCreateRTV(){
            return GetOrCreateRTV(RHITextureViewDesc{
                .format = GetFormat()
            });
        }
        UAV* GetOrCreateUAV(){
            return GetOrCreateUAV(RHITextureViewDesc{
                .format = GetFormat()
            });
        }
        DSV* GetOrCreateDSV(){
            return GetOrCreateDSV(RHITextureViewDesc{
                .format = GetFormat()
            });
        }
    };
}
