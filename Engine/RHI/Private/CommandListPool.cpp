#include "Assert.hpp"
#include "CommandListPool.hpp"
#include "RHICommandList.hpp"
#include "RHIDefinitions.hpp"
#include "RHIDevice.hpp"
#include "RHIFence.hpp"
#include "RHISwapchain.hpp"

namespace Smol
{
    CommandListPool::CommandListPool(RHIDevice& device)
        : device(device){}

    CommandListPool::~CommandListPool() = default;

    void CommandListPool::BeginFrame(){
        frameIndex = (frameIndex + 1) % RHI_FRAMES_IN_FLIGHT;
        auto& slot = slots[frameIndex];

        slot.cmdLists.clear();
        slot.nextIndex = 0;
    }

    RHICommandList* CommandListPool::Acquire(){
        SMOL_ASSERT(frameIndex < RHI_FRAMES_IN_FLIGHT);
        auto& slot = slots[frameIndex];

        if(slot.nextIndex >= slot.cmdLists.size()){
            slot.cmdLists.emplace_back(device.CreateCommandList());
        }

        return slot.cmdLists[slot.nextIndex++].get();
    }

    void CommandListPool::SubmitFrame(
        RHISwapchain* swapchain,
        RHIFence* fence,
        u64 fenceValue
    ){
        SMOL_ASSERT(frameIndex < RHI_FRAMES_IN_FLIGHT);
        auto& slot = slots[frameIndex];

        if(slot.nextIndex == 0){
            auto* cmdList = Acquire();
            cmdList->Begin();
            cmdList->Close();
        }

        if(fence != nullptr)
            device.SignalFence(*slot.cmdLists.back(), *fence, fenceValue);

        for(auto& cmdList: slot.cmdLists){
            device.Submit(*cmdList, swapchain);
        }
    }
}
