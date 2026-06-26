#include <Metal/Metal.hpp>
#include "EnumUtil.hpp"
#include "MetalTexture.hpp"
#include "MetalUtil.hpp"
#include "Primitives.hpp"
#include "RHIDefinitions.hpp"

namespace{
    auto convert(Smol::RHITextureUsage usage){
        using namespace Smol;
        using enum RHITextureUsage;
        MTL::TextureUsage mtlUsage = 0;

        if(hasFlag(usage, AllowShaderRead))
            mtlUsage |= MTL::TextureUsageShaderRead;
        if(hasFlag(usage, AllowRenderTarget))
            mtlUsage |= MTL::TextureUsageRenderTarget;
        if(hasFlag(usage, AllowDepthStencil))
            mtlUsage |= MTL::TextureUsageRenderTarget;
        if(hasFlag(usage, AllowShaderWrite))
            mtlUsage |= MTL::TextureUsageShaderWrite;

        return mtlUsage;
    }
}

namespace Smol
{
    MetalTexture::MetalTexture(
        MTL::Device& device,
        const RHITextureCreateDesc& desc,
        StrView name
    )
        : currentState(desc.initialState)
    {
        auto texDesc = MTL::TextureDescriptor::alloc()->init();
        texDesc->setWidth(desc.width);
        texDesc->setHeight(desc.height);
        texDesc->setDepth(desc.depth);
        texDesc->setMipmapLevelCount(desc.mipLevels);
        texDesc->setArrayLength(desc.arraySize);

        texDesc->setPixelFormat(convert(desc.format));
        texDesc->setTextureType(
            desc.depth > 1 ? MTL::TextureType3D :
                (desc.arraySize > 1 ? MTL::TextureType2DArray
                                    : MTL::TextureType2D)
        );
        texDesc->setUsage(::convert(desc.usage));
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

    MetalTexture::MetalTexture(
        CA::MetalDrawable* drawable
    ){
        texture = drawable->texture();
        texture->retain();
    }

    MetalTexture::~MetalTexture(){
        texture->release();
    }

    void MetalTexture::Upload(
        const void* data,
        u32 mipLevel,
        u32 arraySlice
    ){
        auto bytesPerPixel = getBytesPerPixel(GetFormat());
        auto bytesPerRow = GetWidth() * bytesPerPixel;
        auto region = MTL::Region::Make2D(
            0,
            0,
            GetWidth(),
            GetHeight()
        );

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
