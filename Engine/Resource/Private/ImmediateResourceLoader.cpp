#include "ImageLoader.hpp"
#include "ImmediateResourceLoader.hpp"
#include "RHIDevice.hpp"
#include "RHITexture.hpp"

namespace Smol
{
    ImmediateResourceLoader::ImmediateResourceLoader(
        RHIDevice& device,
        std::filesystem::path root
    )
        : device(device)
        , root(root)
    {}

    void ImmediateResourceLoader::Submit(
        const Request& request, Handle handle
    ){
        auto imagePath = root / request.path;
        auto image = loadImage(imagePath);

        auto texture = device.CreateTexture(
            RHITextureCreateDesc{
                .width = image.GetWidth(), .height = image.GetHeight(),
                .format = RHIPixelFormat::RGBA8_UNORM,
                .usage = RHITextureUsage::AllowShaderRead,
                .initialData = image.GetBufferPointer()
            }
        );
        done.emplace_back(ResourceLoader<SpriteResource>::Completion{
            .data = SpriteResource{
                .texture = std::move(texture),
                .sheetSize = request.sheetSize,
                .animations = request.animations
            },
            .handle = handle
        });
    }

    void ImmediateResourceLoader::Poll(std::vector<Completion>& out){
        out.swap(done);
    }
}
