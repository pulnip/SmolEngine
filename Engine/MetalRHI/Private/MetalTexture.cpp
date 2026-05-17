#include <Metal/Metal.hpp>
#include "EnumUtil.hpp"
#include "MetalTexture.hpp"
#include "MetalUtil.hpp"
#include "Primitives.hpp"

namespace Smol
{
    inline auto convert(RHITextureUsage usage){
        MTL::TextureUsage mtlUsage = 0;

        if(hasFlag(usage, RHITextureUsage::AllowShaderRead))
            mtlUsage |= MTL::TextureUsageShaderRead;
        if(hasFlag(usage, RHITextureUsage::AllowRenderTarget))
            mtlUsage |= MTL::TextureUsageRenderTarget;
        if(hasFlag(usage, RHITextureUsage::AllowDepthStencil))
            mtlUsage |= MTL::TextureUsageRenderTarget;
        if(hasFlag(usage, RHITextureUsage::AllowShaderWrite))
            mtlUsage |= MTL::TextureUsageShaderWrite;

        return mtlUsage;
    }

    MetalTexture::MetalTexture(
        MTL::Device& device,
        const RHITextureCreateDesc& desc,
        StrView name
    )
        : width(desc.width), height(desc.height)
        , format(desc.format)
        , currentState(desc.initialState)
    {
        auto texDesc = MTL::TextureDescriptor::alloc()->init();
        texDesc->setWidth(desc.width);
        texDesc->setHeight(desc.height);
        texDesc->setDepth(desc.depth);
        texDesc->setMipmapLevelCount(desc.mipLevels);
        texDesc->setArrayLength(desc.arraySize);

        texDesc->setPixelFormat(convertPixelFormat(desc.format));
        texDesc->setTextureType(
            desc.depth > 1 ? MTL::TextureType3D :
                (desc.arraySize > 1 ? MTL::TextureType2DArray
                                    : MTL::TextureType2D)
        );
        texDesc->setUsage(convert(desc.usage));
    #if TARGET_OS_OSX
        bool needsGPUOnly = hasFlag(desc.access, RHIMemoryAccess::GPUOnly);
        texDesc->setStorageMode(needsGPUOnly ?
            MTL::StorageModePrivate : MTL::StorageModeShared);
    #else
        texDesc->setStorageMode(MTL::StorageModeShared);
    #endif

        texture = device.newTexture(texDesc);
        texDesc->release();

    #if defined(_DEBUG) || !defined(NDEBUG)
        if(!name.empty()){
            texture->setLabel(
                NS::String::string(name.data(), NS::UTF8StringEncoding)
            );
        }
    #endif

        if(desc.initialData)
            Upload(desc.initialData);
    }

    MetalTexture::~MetalTexture(){
        texture->release();
    }

    void MetalTexture::Upload(
        const void* data,
        u32 mipLevel,
        u32 arraySlice
    ){
        auto bytesPerPixel = getBytesPerPixel(format);
        auto bytesPerRow = width * bytesPerPixel;
        auto region = MTL::Region::Make2D(0, 0, width, height);

        texture->replaceRegion(
            region,
            mipLevel,
            arraySlice,
            data,
            bytesPerRow,
            0
        );
    }
}
