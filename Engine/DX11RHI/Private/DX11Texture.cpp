#include "Assert.hpp"
#include "DX11Definitions.hpp"
#include "DX11Texture.hpp"
#include "DX11Util.hpp"
#include "EnumUtil.hpp"
#include "RHIDefinitions.hpp"

namespace Smol
{
    DX11Texture::DX11Texture(
        Device& device,
        const RHITextureCreateDesc& desc,
        StrView name
    )
        : device(device)
    {
        using enum RHITextureUsage;

        SMOL_ASSERT(desc.depth == 1);

        auto isShaderResource  = hasFlag(desc.usage, ShaderResource);
        auto isRenderTarget    = hasFlag(desc.usage, RenderTarget);
        auto isDepthTarget     = hasFlag(desc.usage, DepthStencil);
        auto isUnorderedAccess = hasFlag(desc.usage, UnorderedAccess);

        UINT bindFlags = 0;
        if(isShaderResource ) bindFlags |= D3D11_BIND_SHADER_RESOURCE;
        if(isRenderTarget   ) bindFlags |= D3D11_BIND_RENDER_TARGET;
        if(isDepthTarget    ) bindFlags |= D3D11_BIND_DEPTH_STENCIL;
        if(isUnorderedAccess) bindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        bool needsGPUOnly = isShaderResource || isRenderTarget ||
                            isDepthTarget    || isUnorderedAccess;

        D3D11_TEXTURE2D_DESC texDesc{
            .Width = desc.width,
            .Height = desc.height,
            .MipLevels = desc.mipLevels,
            .ArraySize = desc.arraySize,
            .Format = convert(desc.format, isShaderResource, isDepthTarget),
            // No MSAA
            .SampleDesc = {1, 0},
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = bindFlags,
            .CPUAccessFlags = 0,
            .MiscFlags = 0
        };
        D3D11_SUBRESOURCE_DATA initData{
            .pSysMem = desc.initialData,
            .SysMemPitch = desc.width * static_cast<UINT>(getBytesPerPixel(desc.format)),
            .SysMemSlicePitch = 0
        };

        CHECK_HRESULT(device.CreateTexture2D(
            &texDesc,
            desc.initialData != nullptr ? &initData : nullptr,
            &texture
        ), "Failed to create DX11 texture");

    #if defined(_DEBUG) || !defined(NDEBUG)
        if(!name.empty()){
            texture->SetPrivateData(
                WKPDID_D3DDebugObjectName,
                static_cast<UINT>(name.length()),
                name.data()
            );
        }
    #endif
    }

    DX11Texture::DX11Texture(
        Device& device,
        Swapchain& swapchain,
        StrView name
    )
        : device(device)
    {
        CHECK_HRESULT(swapchain.GetBuffer(
            0,
            __uuidof(Texture),
            reinterpret_cast<void**>(texture.GetAddressOf())
        ), "Failed to Get Buffer from Swapchain");

    #if defined(_DEBUG) || !defined(NDEBUG)
        if(!name.empty()){
            texture->SetPrivateData(
                WKPDID_D3DDebugObjectName,
                static_cast<UINT>(name.length()),
                name.data()
            );
        }
    #endif
    }

    DX11Texture::~DX11Texture() = default;

    RHIPixelFormat DX11Texture::GetFormat() const noexcept{
        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);

        return convert(desc.Format);
    }

    u32 DX11Texture::GetWidth() const noexcept{
        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);

        return desc.Width;
    }

    u32 DX11Texture::GetHeight() const noexcept{
        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);

        return desc.Height;
    }

    SRV* DX11Texture::GetOrCreateSRV(const RHITextureViewDesc& desc){
        if(auto it = srvs.find(desc); it != srvs.end())
            return it->second.Get();

        const D3D11_SHADER_RESOURCE_VIEW_DESC dxDesc{
            .Format = convert(desc.format, true, false),
            .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
            .Texture2D = {
                .MostDetailedMip = 0,
                .MipLevels = 1
            }
        };

        SRVRAII view;
        CHECK_HRESULT(device.CreateShaderResourceView(
            texture.Get(),
            &dxDesc,
            &view
        ), "Failed to create SRV");

        auto [it, ret] = srvs.emplace(desc, std::move(view));
        SMOL_ASSERT(ret);

        return it->second.Get();
    }

    RTV* DX11Texture::GetOrCreateRTV(const RHITextureViewDesc& desc){
        if(auto it = rtvs.find(desc); it != rtvs.end())
            return it->second.Get();

        const D3D11_RENDER_TARGET_VIEW_DESC dxDesc{
            .Format = convert(desc.format),
            .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
            .Texture2D = {
                .MipSlice = 0
            }
        };

        RTVRAII view;
        CHECK_HRESULT(device.CreateRenderTargetView(
            texture.Get(),
            &dxDesc,
            &view
        ), "Failed to create RTV");

        auto [it, ret] = rtvs.emplace(desc, std::move(view));
        SMOL_ASSERT(ret);

        return it->second.Get();
    }

    UAV* DX11Texture::GetOrCreateUAV(const RHITextureViewDesc& desc){
        if(auto it = uavs.find(desc); it != uavs.end())
            return it->second.Get();

        const D3D11_UNORDERED_ACCESS_VIEW_DESC dxDesc{
            .Format = convert(desc.format),
            .ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D,
            .Texture2D = {
                .MipSlice = 0
            }
        };

        UAVRAII view;
        CHECK_HRESULT(device.CreateUnorderedAccessView(
            texture.Get(),
            &dxDesc,
            &view
        ), "Failed to create UAV");

        auto [it, ret] = uavs.emplace(desc, std::move(view));
        SMOL_ASSERT(ret);

        return it->second.Get();
    }

    DSV* DX11Texture::GetOrCreateDSV(const RHITextureViewDesc& desc){
        if(auto it = dsvs.find(desc); it != dsvs.end())
            return it->second.Get();

        const D3D11_DEPTH_STENCIL_VIEW_DESC dxDesc{
            .Format = convert(desc.format, false, true),
            .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
            .Texture2D = {
                .MipSlice = 0
            }
        };

        DSVRAII view;
        CHECK_HRESULT(device.CreateDepthStencilView(
            texture.Get(),
            &dxDesc,
            &view
        ), "Failed to create DSV");

        auto [it, ret] = dsvs.emplace(desc, std::move(view));
        SMOL_ASSERT(ret);

        return it->second.Get();
    }
}
