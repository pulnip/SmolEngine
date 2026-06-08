#pragma once

#include <filesystem>
#include <unordered_map>
#include "Primitives.hpp"
#include "RHIFWD.hpp"

namespace Smol
{
    template<typename T>
    concept ResourceRequest = requires(T t){
        typename T::Key;
        typename T::KeyHash;
        { t.key() } -> std::same_as<typename T::Key>;
    };

    template<typename T>
    concept Resource = requires{
        typename T::Request;
        requires ResourceRequest<typename T::Request>;
    };

    struct SpriteAnimation{
        u32 startRow = 0;
        u32 startCol, frameCount = 1;
        f32 frameDurationMs = 160.0f;
    };

    struct SpriteRequest{
        using Key = std::filesystem::path;
        using KeyHash = std::hash<Key>;

        Key path;
        Size2D sheetSize;
        std::unordered_map<Str, SpriteAnimation> animations;

        Key key() const noexcept{ return path; }
    };

    struct SpriteResource{
        using Request = SpriteRequest;

        RHITextureRAII texture;
        Size2D sheetSize;
        std::unordered_map<Str, SpriteAnimation> animations;
    };
    static_assert(Resource<SpriteResource>);
}
