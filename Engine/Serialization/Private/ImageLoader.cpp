#include <cstring>
#include <utility>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Assert.hpp"
#include "ImageLoader.hpp"
#include "LogLocal.hpp"
#include "StringUtil.hpp"

namespace{
    auto convert(Smol::ImageFormat format){
        using enum Smol::ImageFormat;

        switch(format){
        case RGBA:
            return STBI_rgb_alpha;
        default:
            std::unreachable();
        }
    }

}

namespace Smol
{
    u32 toChannels(ImageFormat format) noexcept{
        using enum ImageFormat;

        switch(format){
        case RGBA:
            return 4;
        default:
            std::unreachable();
        }
    }

    ImageData loadImage(const std::filesystem::path& path, ImageFormat desiredFormat){
        auto bin = readFileAsBinary(path);

        int width = 0, height = 0, channels = 0;
        auto data = stbi_load_from_memory(
            bin.data(), static_cast<int>(bin.size()),
            &width, &height, &channels,
            ::convert(desiredFormat)
        );

        SMOL_ASSERT(toChannels(desiredFormat) == channels);
        if(data == nullptr){
            LOG_ERROR("Image load Failed: {} - {}",
                path, stbi_failure_reason()
            );
        }

        auto size = width * height * channels;
        ImageBlob blob(size);
        std::memcpy(blob.data(), data, size);

        stbi_image_free(data);

        return ImageData(std::move(blob),
            static_cast<u32>(width),
            static_cast<u32>(height),
            desiredFormat
        );
    }
}
