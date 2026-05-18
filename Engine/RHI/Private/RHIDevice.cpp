#include "CommandListPool.hpp"
#include "FramePacer.hpp"
#include "RHIDevice.hpp"

namespace Smol
{
    class RHIDevice::Impl{
    private:
        FramePacer framePacer;
        CommandListPool cmdListPool;

    public:
        Impl(RHIDevice&);
        ~Impl();

        void BeginFrame();
        void EndFrame(RHIDevice&);
    };

    RHIDevice::RHIDevice()
        : impl(std::make_unique<Impl>(*this)){}

    RHIDevice::Impl::Impl(RHIDevice& device)
        : framePacer(device)
        , cmdListPool(device){}

    RHIDevice::~RHIDevice() = default;

    RHIDevice::Impl::~Impl(){
        framePacer.WaitForIdle();
    }

    void RHIDevice::BeginFrame(){
        impl->BeginFrame();
    }

    void RHIDevice::Impl::BeginFrame(){
        framePacer.BeginFrame();

        auto frameIndex = framePacer.GetCurrentFrameIndex();
        cmdListPool.BeginFrame(frameIndex);
    }

    void RHIDevice::EndFrame(){
        impl->EndFrame(*this);
    }

    void RHIDevice::Impl::EndFrame(RHIDevice& device){
        auto frameIndex = framePacer.GetCurrentFrameIndex();
        cmdListPool.Submit(frameIndex);

        device.SignalFence(
            *framePacer.GetCurrentFence(),
            framePacer.GetNextFenceValue()
        );

        framePacer.EndFrame();
    }
}
