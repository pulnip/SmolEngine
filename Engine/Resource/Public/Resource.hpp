#pragma once

#include <filesystem>
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

    struct SpriteRequest{
        using Key = std::filesystem::path;
        using KeyHash = std::hash<Key>;

        Key path;
        const u32 frameCount = 1;
        const f32 framePerSeconds = 0.16f;

        Key key() const noexcept{ return path; }
    };

    struct SpriteResource{
        using Request = SpriteRequest;

        RHITextureRAII texture;

        u32 frameCount = 1;
        f32 framePerSeconds = 0.16f;
    };
    static_assert(Resource<SpriteResource>);
}
