#include "Log.hpp"
#include "RHIDevice.hpp"
#include "RHIFence.hpp"
#include "RHIFrameScope.hpp"
#include "FramePacer.hpp"

namespace Smol
{
    // Helper class for managing per-frame fences
    // Used for triple buffering to synchronize CPU and GPU work
    class RHIFrameFenceManager{
    private:
        RHIDevice& device;
        RHIFenceRAII fence;
        u64 currentFenceValue = 0;
        u32 currentFrame = 0;

    public:
        RHIFrameFenceManager(RHIDevice& device)
            : device(device)
            , fence(device.CreateFence(0))
        {
            LOG_INFO("RHI",
                "Created frame fence manager with {} frames in flight",
                RHI_FRAMES_IN_FLIGHT
            );
        }

        ~RHIFrameFenceManager(){
            // Wait for all fences before destruction
            WaitForAll();
        }

        // Begin a new frame
        // Waits for N-2 frame to ensure GPU is done with it
        void BeginFrame(){
            if(currentFenceValue >= RHI_FRAMES_IN_FLIGHT){
                auto waitValue = currentFenceValue - RHI_FRAMES_IN_FLIGHT;
                fence->WaitCPU(waitValue);
            }
        }

        // End the current frame
        // Signals the fence so GPU can indicate when work is done
        void EndFrame(){
            // Increment fence value for next wait
            ++currentFenceValue;

            // Move to next frame
            currentFrame = (currentFrame + 1) % RHI_FRAMES_IN_FLIGHT;
        }

        // Wait for all frames to complete
        void WaitForAll(){
            if(currentFenceValue > 0){
                fence->WaitCPU(currentFenceValue);
            }

            LOG_INFO("RHI",
                "Waited for all {} frames to complete", RHI_FRAMES_IN_FLIGHT
            );
        }

        // Get current frame index
        u32 GetCurrentFrameIndex() const noexcept{
            return currentFrame;
        }

        // Get fence for current frame
        RHIFence* GetCurrentFence() const noexcept{
            return fence.get();
        }

        // Get fence value for current frame
        u64 GetCurrentFenceValue() const noexcept{
            return currentFenceValue;
        }
    };

    class FramePacer::Impl{
    private:
        RHIDevice& device;
        RHIFrameScopeRAII scope = nullptr;
        RHIFrameFenceManager fenceManager;

        u64 frameNumber = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime = std::chrono::high_resolution_clock::now();
        f64 deltaTime = 0.0;
        f64 fps = 0.0;
        f64 frameTimeAccum = 0.0;
        u32 frameCount = 0;

    public:
        Impl(RHIDevice& device)
            : device(device)
            , fenceManager(device)
        {}

        ~Impl() = default;

        bool BeginFrame(){
            scope = device.CreateFrameScope();

            // Wait for oldest frame to complete
            fenceManager.BeginFrame();

            // Update timing
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<f64> elapsed = currentTime - lastFrameTime;
            deltaTime = elapsed.count();
            lastFrameTime = currentTime;

            // Update FPS counter
            frameTimeAccum += deltaTime;
            ++frameCount;

            if(frameTimeAccum >= 1.0){
                fps = static_cast<f64>(frameCount) / frameTimeAccum;
                frameTimeAccum = 0.0;
                frameCount = 0;

                LOG_DEBUG("RHI",
                    "FPS: {:.1f}, Frame Time: {:.2f}ms",
                    fps, deltaTime * 1000.0
                );
            }

            ++frameNumber;
            return true;
        }

        void EndFrame(){
            scope = nullptr;

            // Signal fence for this frame
            fenceManager.EndFrame();
        }

        void WaitForIdle(){
            fenceManager.WaitForAll();
            LOG_INFO("RHI", "Frame pacer idle");
        }

        u32 GetCurrentFrameIndex() const noexcept{
            return fenceManager.GetCurrentFrameIndex();
        }

        u64 GetFrameNumber() const noexcept{ return frameNumber; }
        f64 GetDeltaTime() const noexcept{ return deltaTime; }
        f64 GetFPS() const noexcept{ return fps; }
        f64 GetFrameTimeMs() const noexcept{ return deltaTime * 1000.0; }

        auto GetCurrentFence(this auto& self) noexcept{
            return self.fenceManager.GetCurrentFence();
        }

        u64 GetNextFenceValue() const noexcept{
            // Return the next fence value that will be used after endFrame()
            return fenceManager.GetCurrentFenceValue() + 1;
        }
    };

    FramePacer::FramePacer(RHIDevice& device)
        : impl(std::make_unique<Impl>(device)){}

    FramePacer::~FramePacer() = default;

    bool FramePacer::BeginFrame(){
        return impl->BeginFrame();
    }

    void FramePacer::EndFrame(){
        impl->EndFrame();
    }

    void FramePacer::WaitForIdle(){
        impl->WaitForIdle();
    }

    u32 FramePacer::GetCurrentFrameIndex() const{
        return impl->GetCurrentFrameIndex();
    }
    u64 FramePacer::GetFrameNumber() const{
        return impl->GetFrameNumber();
    }
    f64 FramePacer::GetDeltaTime() const{
        return impl->GetDeltaTime();
    }
    f64 FramePacer::GetFPS() const{
        return impl->GetFPS();
    }
    f64 FramePacer::GetFrameTimeMs() const{
        return impl->GetFrameTimeMs();
    }

    RHIFence* FramePacer::GetCurrentFence(){
        return impl->GetCurrentFence();
    }
    const RHIFence* FramePacer::GetCurrentFence() const{
        return impl->GetCurrentFence();
    }

    u64 FramePacer::GetNextFenceValue() const{
        return impl->GetNextFenceValue();
    }
}
