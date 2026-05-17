#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Smol
{
    using Device = ID3D11Device;
    using DeviceContext = ID3D11DeviceContext;
    using Factory = IDXGIFactory2;
    using Adapter = IDXGIAdapter1;
    using Swapchain = IDXGISwapChain1;

    // Pipeline state types
    // Input Assembler Stage
    using InputLayout = ID3D11InputLayout;
    // Vertex Shader Stage
    using VertexShader = ID3D11VertexShader;
    // Rasterizer Stage
    using RasterizerState = ID3D11RasterizerState;
    // Pixel Shader Stage
    using PixelShader = ID3D11PixelShader;
    // Output Merger Stage
    using DepthStencilState = ID3D11DepthStencilState;
    using BlendState = ID3D11BlendState;

    // Compute Shader Stage
    using ComputeShader = ID3D11ComputeShader;

    // Resource types
    using Buffer = ID3D11Buffer;
    using Texture = ID3D11Texture2D;
    using Sampler = ID3D11SamplerState;

    // View types
    using SRV = ID3D11ShaderResourceView;
    using RTV = ID3D11RenderTargetView;
    using UAV = ID3D11UnorderedAccessView;
    using DSV = ID3D11DepthStencilView;

    // RAII wrappers for COM interfaces
    template<typename T>
    using COMRAII = Microsoft::WRL::ComPtr<T>;

    using DeviceRAII = COMRAII<Device>;
    using DeviceContextRAII = COMRAII<DeviceContext>;
    using FactoryRAII = COMRAII<Factory>;
    using AdapterRAII = COMRAII<Adapter>;
    using SwapchainRAII = COMRAII<Swapchain>;

    using InputLayoutRAII = COMRAII<InputLayout>;
    using RasterizerStateRAII = COMRAII<RasterizerState>;
    using DepthStencilStateRAII = COMRAII<DepthStencilState>;
    using BlendStateRAII = COMRAII<BlendState>;
    using VertexShaderRAII = COMRAII<VertexShader>;
    using PixelShaderRAII = COMRAII<PixelShader>;

    using ComputeShaderRAII = COMRAII<ComputeShader>;

    using BufferRAII = COMRAII<Buffer>;
    using TextureRAII = COMRAII<Texture>;
    using SamplerRAII = COMRAII<Sampler>;

    using SRVRAII = COMRAII<SRV>;
    using RTVRAII = COMRAII<RTV>;
    using UAVRAII = COMRAII<UAV>;
    using DSVRAII = COMRAII<DSV>;
}
