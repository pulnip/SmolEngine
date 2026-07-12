#include <utility>
#include "Assert.hpp"
#include "DX11Definitions.hpp"
#include "EnumUtil.hpp"
#include "PtrUtil.hpp"
#include "RHIDefinitions.hpp"
#include "VariantUtil.hpp"
#include "DX11Buffer.hpp"
#include "DX11Util.hpp"

namespace{
    struct BufferPolicy{
        D3D11_USAGE usage;
        UINT CPUAccessFlag;
    };

    auto Resolve(
        Smol::RHIMemoryAccess access,
        Smol::RHIBufferUsage usage
    ){
        using namespace Smol;
        using enum RHIMemoryAccess;
        using enum RHIBufferUsage;

        const auto isUnorderedAccess = hasFlag(usage, UnorderedAccess);
        const auto isGPUOnly = (access == GPUOnly);
        SMOL_ASSERT(!isUnorderedAccess || isGPUOnly);

        switch(access){
        case GPUOnly:
            return BufferPolicy{
                .usage = D3D11_USAGE_DEFAULT,
                .CPUAccessFlag = 0
            };
        case CPUWrite:
            return BufferPolicy{
                .usage = D3D11_USAGE_DYNAMIC,
                .CPUAccessFlag = D3D11_CPU_ACCESS_WRITE
            };
        case CPURead:
            return BufferPolicy{
                .usage = D3D11_USAGE_STAGING,
                .CPUAccessFlag = D3D11_CPU_ACCESS_READ
            };
        default:
            std::unreachable();
        }
    }
}

namespace Smol
{
    DX11Buffer::DX11Buffer(
        Device& device,
        DeviceContext& context,
        const RHIBufferCreateDesc& desc,
        StrView name
    )
        : device(device), context(context)
    {
        using enum RHIBufferUsage;
        using enum RHIMemoryAccess;

        UINT bindFlags = 0;
        if(hasFlag(desc.usage, VertexBuffer))
            bindFlags |= D3D11_BIND_VERTEX_BUFFER;
        if(hasFlag(desc.usage, IndexBuffer))
            bindFlags |= D3D11_BIND_INDEX_BUFFER;
        if(hasFlag(desc.usage, ConstantBuffer))
            bindFlags |= D3D11_BIND_CONSTANT_BUFFER;

        UINT miscFlags = 0;
        // Shader Read
        if(hasFlag(desc.usage, ShaderResource)){
            bindFlags |= D3D11_BIND_SHADER_RESOURCE;
            // ByteAddressBuffer in HLSL
            miscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
        }
        // Shader Write
        if(hasFlag(desc.usage, UnorderedAccess)){
            bindFlags |= D3D11_BIND_UNORDERED_ACCESS;
            // RWByteAddressBuffer in HLSL
            miscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
        }
        if(hasFlag(desc.usage, IndirectArgument)){
            miscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
        }

        const auto policy = ::Resolve(desc.access, desc.usage);

        D3D11_BUFFER_DESC dxDesc = {
            .ByteWidth = static_cast<UINT>(desc.size),
            .Usage = policy.usage,
            .BindFlags = bindFlags,
            .CPUAccessFlags = policy.CPUAccessFlag,
            .MiscFlags = miscFlags,
            .StructureByteStride = 0
        };

        D3D11_SUBRESOURCE_DATA initData{
            .pSysMem = desc.initialData
        };

        CHECK_HRESULT(device.CreateBuffer(
            &dxDesc,
            desc.initialData != nullptr ? &initData : nullptr,
            &buffer
        ), "Failed to create DX11 buffer");

        #if defined(_DEBUG) || !defined(NDEBUG)
            if(!name.empty()){
                buffer->SetPrivateData(
                    WKPDID_D3DDebugObjectName,
                    static_cast<UINT>(name.length()),
                    name.data()
                );
            }
        #endif
        }

        DX11Buffer::~DX11Buffer() = default;

        void DX11Buffer::Upload(
            const void* src,
            u32 srcSize,
            u32 offset
        ){
            SMOL_ASSERT(srcSize <= GetSize() - offset);

            D3D11_MAPPED_SUBRESOURCE mapped;
            CHECK_HRESULT(context.Map(
                buffer.Get(),
                0,
                D3D11_MAP_WRITE_DISCARD,
                0,
                &mapped
            ), "Failed to Map DX11 Buffer");

            std::memcpy(
                ptrAdd(mapped.pData, offset),
                src,
                srcSize
            );

            context.Unmap(
                buffer.Get(),
                0
            );
        }

    void DX11Buffer::Download(
        void* dst,
        u32 dstSize,
        u32 offset
    ){
        SMOL_ASSERT(dstSize <= GetSize() - offset);

        D3D11_MAPPED_SUBRESOURCE mapped;
        CHECK_HRESULT(context.Map(
            buffer.Get(),
            0,
            D3D11_MAP_READ,
            0,
            &mapped
        ), "Failed to Map DX11 Buffer");

        std::memcpy(
            dst,
            mapped.pData,
            dstSize
        );

        context.Unmap(
            buffer.Get(),
            0
        );
    }

    u32 DX11Buffer::GetSize() const noexcept{
        D3D11_BUFFER_DESC desc;
        buffer->GetDesc(&desc);

        return desc.ByteWidth;
    }

    SRV* DX11Buffer::GetOrCreateSRV(const RHIBufferViewDesc& desc){
        if(auto it = srvs.find(desc); it != srvs.end())
            return it->second.Get();

        const auto dxDesc = std::visit(overload{
            [&desc](const RHIBufferViewDesc::RawConfig&){
                constexpr u32 RAW_STRIDE = 4;
                return D3D11_SHADER_RESOURCE_VIEW_DESC{
                    .Format = DXGI_FORMAT_R32_TYPELESS,
                    .ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX,
                    .BufferEx = {
                        .FirstElement = desc.offset / RAW_STRIDE,
                        .NumElements = desc.size / RAW_STRIDE,
                        .Flags = D3D11_BUFFEREX_SRV_FLAG_RAW,
                    }
                };
            },
            [&desc](const RHIBufferViewDesc::TypedConfig& c){
                const u32 bpp = getBytesPerPixel(c.format);
                return D3D11_SHADER_RESOURCE_VIEW_DESC{
                    .Format = convert(c.format),
                    .ViewDimension = D3D11_SRV_DIMENSION_BUFFER,
                    .Buffer = {
                        .FirstElement = desc.offset / bpp,
                        .NumElements = desc.size / bpp,
                    },
                };
            },
            [desc](const RHIBufferViewDesc::StructuredConfig& c){
                return D3D11_SHADER_RESOURCE_VIEW_DESC{
                    .Format = DXGI_FORMAT_UNKNOWN,
                    .ViewDimension = D3D11_SRV_DIMENSION_BUFFER,
                    .Buffer = {
                        .FirstElement = desc.offset / c.stride,
                        .NumElements = desc.size / c.stride,
                    },
                };
            }
        }, desc.config);

        SRVRAII view;
        CHECK_HRESULT(device.CreateShaderResourceView(
            buffer.Get(),
            &dxDesc,
            &view
        ), "Failed to create SRV");

        auto [it, ret] = srvs.emplace(desc, std::move(view));
        SMOL_ASSERT(ret);

        return it->second.Get();
    }

    UAV* DX11Buffer::GetOrCreateUAV(const RHIBufferViewDesc& desc){
        if(auto it = uavs.find(desc); it != uavs.end())
            return it->second.Get();

        const auto dxDesc = std::visit(overload{
            [&desc](const RHIBufferViewDesc::RawConfig&){
                constexpr u32 RAW_STRIDE = 4;
                return D3D11_UNORDERED_ACCESS_VIEW_DESC{
                    .Format = DXGI_FORMAT_R32_TYPELESS,
                    .ViewDimension = D3D11_UAV_DIMENSION_BUFFER,
                    .Buffer = {
                        .FirstElement = desc.offset / RAW_STRIDE,
                        .NumElements = desc.size / RAW_STRIDE,
                        .Flags = D3D11_BUFFER_UAV_FLAG_RAW
                    }
                };
            },
            [&desc](const RHIBufferViewDesc::TypedConfig& c){
                const u32 bpp = getBytesPerPixel(c.format);
                return D3D11_UNORDERED_ACCESS_VIEW_DESC{
                    .Format = convert(c.format),
                    .ViewDimension = D3D11_UAV_DIMENSION_BUFFER,
                    .Buffer = {
                        .FirstElement = desc.offset / bpp,
                        .NumElements = desc.size / bpp,
                        .Flags = 0
                    },
                };
            },
            [desc](const RHIBufferViewDesc::StructuredConfig& c){
                return D3D11_UNORDERED_ACCESS_VIEW_DESC{
                    .Format = DXGI_FORMAT_UNKNOWN,
                    .ViewDimension = D3D11_UAV_DIMENSION_BUFFER,
                    .Buffer = {
                        .FirstElement = desc.offset / c.stride,
                        .NumElements = desc.size / c.stride,
                        .Flags = D3D11_BUFFER_UAV_FLAG_COUNTER
                    },
                };
            }
        }, desc.config);

        UAVRAII view;
        CHECK_HRESULT(device.CreateUnorderedAccessView(
            buffer.Get(),
            &dxDesc,
            &view
        ), "Failed to create UAV");
        auto [it, ret] = uavs.emplace(desc, std::move(view));
        SMOL_ASSERT(ret);

        return it->second.Get();
    }
}
