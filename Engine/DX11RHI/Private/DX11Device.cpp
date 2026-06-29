#include <stdexcept>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include "DX11Buffer.hpp"
#include "DX11CommandList.hpp"
#include "DX11Definitions.hpp"
#include "DX11Device.hpp"
#include "DX11Fence.hpp"
#include "DX11FrameScope.hpp"
#include "DX11PipelineState.hpp"
#include "DX11Sampler.hpp"
#include "DX11Swapchain.hpp"
#include "DX11Texture.hpp"

namespace Smol
{
    RHIDeviceRAII CreateDX11Device(){
        return std::make_unique<DX11Device>();
    }

    class DX11Device::Impl{
    private:
        FactoryRAII factory = nullptr;
        DeviceRAII device = nullptr;
        DeviceContextRAII context = nullptr;

    public:
        Impl(){
            UINT dxgiFactoryFlags = 0;
        #if defined(_DEBUG) || !defined(NDEBUG)
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        #endif

            if(FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)))){
                throw std::runtime_error("Failed to create DXGI factory");
            }

            AdapterRAII adapter, selectedAdapter;
            SIZE_T maxDedicatedMemory = 0;

            for(UINT i=0; factory->EnumAdapters1(i, &adapter)!=DXGI_ERROR_NOT_FOUND; ++i){
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                    continue;

                // Check if adapter supports DX11
                if(SUCCEEDED(D3D11CreateDevice(
                    adapter.Get(),
                    D3D_DRIVER_TYPE_UNKNOWN,
                    nullptr,
                    0,
                    nullptr, 0,
                    D3D11_SDK_VERSION,
                    nullptr,
                    nullptr,
                    nullptr
                ))){
                    if(desc.DedicatedVideoMemory > maxDedicatedMemory){
                        maxDedicatedMemory = desc.DedicatedVideoMemory;
                        selectedAdapter = adapter;
                    }
                }
            }

            if(selectedAdapter == nullptr){
                throw std::runtime_error("No compatible DX11 adapter found");
            }

            UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        #if defined(_DEBUG) || !defined(NDEBUG)
            deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif
            D3D_FEATURE_LEVEL featureLevels[] = {
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
            }, actualLevel;

            if(FAILED(D3D11CreateDevice(
                selectedAdapter.Get(),
                D3D_DRIVER_TYPE_UNKNOWN,
                nullptr,
                deviceFlags,
                featureLevels,
                _countof(featureLevels),
                D3D11_SDK_VERSION,
                device.GetAddressOf(),
                &actualLevel,
                // take immediate context later
                nullptr
            ))){
                throw std::runtime_error("Failed to create DX11 device");
            }

            device->GetImmediateContext(&context);
        }

        ~Impl()= default;

        RHIFrameScopeRAII CreateFrameScopoe() noexcept{
            return std::make_unique<DX11FrameScope>();
        }

        RHIBufferRAII CreateBuffer(
            const RHIBufferCreateDesc& desc,
            StrView name
        ){
            return std::make_unique<DX11Buffer>(
                *device.Get(), *context.Get(),
                desc, name
            );
        }

        RHITextureRAII CreateTexture(
            const RHITextureCreateDesc& desc,
            StrView name
        ){
            return std::make_unique<DX11Texture>(
                *device.Get(),
                desc, name
            );
        }

        RHISamplerRAII CreateSampler(
            const RHISamplerState& desc
        ){
            return std::make_unique<DX11Sampler>(*device.Get(), desc);
        }

        RHIGraphicsPipelineStateRAII CreatePipelineState(
            const RHIGraphicsPipelineStateDesc& desc,
            StrView name
        ){
            if(std::get_if<RHILegacyFrontendDesc>(&desc.preRasterizer)){
                return std::make_unique<DX11GraphicsPipelineState>(*device.Get(), desc, name);
            }
            else{
                // cannot use Mesh Shader in DirectX 11
                throw std::runtime_error("Unsupported Graphics Pipeline Frontend");
            }
        }

        RHIComputePipelineStateRAII CreatePipelineState(
            const RHIComputePipelineStateDesc& desc,
            StrView name
        ){
            return std::make_unique<DX11ComputePipelineState>(*device.Get(), desc, name);
        }

        RHISwapchainRAII CreateSwapchain(
            const RHISwapchainCreateDesc& desc
        ){
            return std::make_unique<DX11Swapchain>(*device.Get(), *factory.Get(), desc);
        }

        RHICommandListRAII CreateCommandList(){
            return std::make_unique<DX11CommandList>(*device.Get(), *context.Get());
        }

        RHIFenceRAII CreateFence(u64 initialValue){
            return std::make_unique<DX11Fence>(*device.Get(), initialValue);
        }

        Device* Get() noexcept{ return device.Get(); }
        DeviceContext* GetContext() noexcept{ return context.Get(); }
    };

    DX11Device::DX11Device()
        : impl(std::make_unique<Impl>()){}

    DX11Device::~DX11Device(){}

    RHIFrameScopeRAII DX11Device::CreateFrameScope(){
        return impl->CreateFrameScopoe();
    }

    RHIBufferRAII DX11Device::CreateBuffer(
        const RHIBufferCreateDesc& desc,
        StrView name
    ){
        return impl->CreateBuffer(desc, name);
    }

    RHITextureRAII DX11Device::CreateTexture(
        const RHITextureCreateDesc& desc,
        StrView name
    ){
        return impl->CreateTexture(desc, name);
    }

    RHISamplerRAII DX11Device::CreateSampler(
        const RHISamplerState& desc
    ){
        return impl->CreateSampler(desc);
    }

    RHIGraphicsPipelineStateRAII DX11Device::CreatePipelineState(
        const RHIGraphicsPipelineStateDesc& desc,
        StrView name
    ){
        return impl->CreatePipelineState(desc, name);
    }

    RHIComputePipelineStateRAII DX11Device::CreatePipelineState(
        const RHIComputePipelineStateDesc& desc,
        StrView name
    ){
        return impl->CreatePipelineState(desc, name);
    }

    RHISwapchainRAII DX11Device::CreateSwapchain(
        const RHISwapchainCreateDesc& desc
    ){
        return impl->CreateSwapchain(desc);
    }

    RHICommandListRAII DX11Device::CreateCommandList(){
        return impl->CreateCommandList();
    }

    RHIFenceRAII DX11Device::CreateFence(u64 initialValue){
        return impl->CreateFence(initialValue);
    }

    RHICapabilities DX11Device::GetCapabilities() const noexcept{
        return {
            .flipTextureV = true,
            .clipSpaceMinZ = 0.0f
        };
    }

    void DX11Device::Submit(RHICommandList&, RHISwapchain* swapchain){
        if(swapchain != nullptr){
            auto& dxSwapchain = static_cast<DX11Swapchain&>(*swapchain);
            dxSwapchain.Present();
        }
    }

    void* DX11Device::Get() noexcept{
        return impl->Get();
    }
    void* DX11Device::GetContextOrQueue() noexcept{
        return impl->GetContext();
    }
}
