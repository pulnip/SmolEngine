#pragma once

#include <array>
#include <vector>
#include "Primitives.hpp"
#include "RHIDefinitions.hpp"
#include "RHIFWD.hpp"

namespace Smol
{
    class CommandListPool{
    private:
        RHIDevice& device;
        struct FrameSlot{
            std::vector<RHICommandListRAII> cmdLists;
            usize nextIndex = 0;
        };
        std::array<FrameSlot, RHI_FRAMES_IN_FLIGHT> slots;

    public:
        CommandListPool(RHIDevice&);
        ~CommandListPool();

        void BeginFrame(u32 frameIndex);

        RHICommandList* Acquire(u32 frameIndex);

        void Submit(u32 frameIndex);
    };
}