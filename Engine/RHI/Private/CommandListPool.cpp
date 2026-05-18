#include "Assert.hpp"
#include "RHIDefinitions.hpp"
#include "CommandListPool.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"

namespace Smol
{
    CommandListPool::CommandListPool(RHIDevice& device)
        : device(device){}

    CommandListPool::~CommandListPool() = default;

    void CommandListPool::BeginFrame(u32 frameIndex){
        SMOL_ASSERT(frameIndex < RHI_FRAMES_IN_FLIGHT);
        auto& slot = slots[frameIndex];

        slot.cmdLists.clear();
        slot.nextIndex = 0;
    }

    RHICommandList* CommandListPool::Acquire(u32 frameIndex){
        SMOL_ASSERT(frameIndex < RHI_FRAMES_IN_FLIGHT);
        auto& slot = slots[frameIndex];

        if(slot.nextIndex >= slot.cmdLists.size()){
            slot.cmdLists.emplace_back(device.CreateCommandList());
        }

        return slot.cmdLists[slot.nextIndex++].get();
    }

    void CommandListPool::Submit(u32 frameIndex){
        SMOL_ASSERT(frameIndex < RHI_FRAMES_IN_FLIGHT);
        auto& slot = slots[frameIndex];

        for(auto& cmdList: slot.cmdLists){
            device.Submit(*cmdList);
        }
    }
}