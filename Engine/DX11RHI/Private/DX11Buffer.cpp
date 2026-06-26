#include <stdexcept>
#include <d3dcommon.h>
#include <d3d11.h>
#include "Assert.hpp"
#include "DX11Definitions.hpp"
#include "EnumUtil.hpp"
#include "PtrUtil.hpp"
#include "VariantUtil.hpp"
#include "DX11Buffer.hpp"
#include "DX11Util.hpp"

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

        const auto hasVertexUsage = hasFlag(desc.usage, VertexBuffer);
        const auto hasIndexUsage = hasFlag(desc.usage, IndexBuffer);
        const auto hasConstantUsage = hasFlag(desc.usage, ConstantBuffer);
        const auto isShaderResource = hasFlag(desc.usage, AllowShaderRead);
        const auto isUnorderedAccess = hasFlag(desc.usage, AllowShaderWrite);

        const auto isCPUWrite = (desc.access == RHIMemoryAccess::CPUWrite);
        const auto isCPURead  = (desc.access == RHIMemoryAccess::CPURead);

        UINT bindFlags = 0;
        if(hasVertexUsage)
            bindFlags |= D3D11_BIND_VERTEX_BUFFER;
        if(hasIndexUsage)
            bindFlags |= D3D11_BIND_INDEX_BUFFER;
        if(hasConstantUsage)
            bindFlags |= D3D11_BIND_CONSTANT_BUFFER;
        if(isShaderResource)
            bindFlags |= D3D11_BIND_SHADER_RESOURCE;
        if(isUnorderedAccess)
            bindFlags |= D3D11_BIND_UNORDERED_ACCESS;

        UINT miscFlags = 0;
        // Raw buffer views require this flag (ByteAddressBuffer / RWByteAddressBuffer)
        if(isShaderResource || isUnorderedAccess)
            miscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
        if(hasFlag(desc.usage, IndirectArgs))
            miscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

        // DYNAMIC+UAV is invalid in DX11; use DEFAULT when UAV binding is needed
        const auto isDynamicUsage = isCPUWrite && !isUnorderedAccess;

        D3D11_BUFFER_DESC dxDesc = {
            .ByteWidth = static_cast<UINT>(desc.size),
            .Usage = isDynamicUsage ?
                D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
            .BindFlags = bindFlags,
            .CPUAccessFlags = isDynamicUsage ?
                D3D11_CPU_ACCESS_WRITE : UINT(0),
            .MiscFlags = miscFlags,
            .StructureByteStride = 0
        };

        D3D11_SUBRESOURCE_DATA initData{
            .pSysMem = desc.initialData
        };

        if(FAILED(device.CreateBuffer(
            &dxDesc,
            desc.initialData != nullptr ? &initData : nullptr,
            &buffer
        ))){
            throw std::runtime_error("Failed to create DX11 buffer");
        }

        // Create a staging buffer for CPU readback when access == CPURead
        if(isCPURead){
            D3D11_BUFFER_DESC stagingDesc{
                .ByteWidth = static_cast<UINT>(desc.size),
                .Usage = D3D11_USAGE_STAGING,
                .BindFlags = 0,
                .CPUAccessFlags = D3D11_CPU_ACCESS_READ,
                .MiscFlags = 0,
                .StructureByteStride = 0
            };
            if(FAILED(device.CreateBuffer(
                &stagingDesc,
                nullptr,
                &stagingBuffer
            ))){
                throw std::runtime_error("Failed to create staging buffer for readback");
            }
        }

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
            context.Map(
                buffer.Get(),
                0,
                D3D11_MAP_WRITE_DISCARD,
                0,
                &mapped
            );

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
        SMOL_ASSERT(stagingBuffer != nullptr,
            "download() requires RHIMemoryAccess::CPURead"
        );

        // Copy GPU buffer → staging, then Map staging for CPU read
        const D3D11_BOX srcBox{
            .left = offset,
            .top = 0,
            .front = 0,
            .right = offset + dstSize,
            .bottom = 1,
            .back = 1
        };
        context.CopySubresourceRegion(
            stagingBuffer.Get(),
            0,
            0, 0, 0,
            buffer.Get(),
            0,
            &srcBox
        );

        D3D11_MAPPED_SUBRESOURCE mapped;
        context.Map(
            stagingBuffer.Get(),
            0,
            D3D11_MAP_READ,
            0,
            &mapped
        );

        std::memcpy(
            dst,
            mapped.pData,
            dstSize
        );

        context.Unmap(
            stagingBuffer.Get(),
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
        device.CreateShaderResourceView(
            buffer.Get(),
            &dxDesc,
            &view
        );

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
        device.CreateUnorderedAccessView(
            buffer.Get(),
            &dxDesc,
            &view
        );
        auto [it, ret] = uavs.emplace(desc, std::move(view));
        SMOL_ASSERT(ret);

        return it->second.Get();
    }
}
