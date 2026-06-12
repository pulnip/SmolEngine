#pragma once

#include <filesystem>
#include "ResourceLoader.hpp"
#include "RHIFWD.hpp"
#include "Resource.hpp"

namespace Smol
{
    class ImmediateResourceLoader: public ResourceLoader<SpriteResource>{
    private:
        RHIDevice& device;
        const std::filesystem::path root;
        std::vector<Completion> done;

    public:
        ImmediateResourceLoader(RHIDevice&, std::filesystem::path root = "Content/");

        void Submit(const Request&, Handle, ResourceManager<SpriteResource>&) override;
        void Poll(std::vector<Completion>&) override{}
    };
}
