#pragma once

#include "CoreFWD.hpp"
#include "Primitives.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    // Frame pacing and synchronization system
    // Manages triple buffering, frame timing, and CPU-GPU synchronization
    class FramePacer{
    private:
        class Impl;
        RAII<Impl> impl;

    public:
        FramePacer(RHIDevice& device);
        ~FramePacer();
        SMOL_DECLARE_MOVE_ONLY(FramePacer)

        // Begin a new frame
        // Returns true if ready to render, false if should skip
        bool BeginFrame();

        // End the current frame
        void EndFrame();

        // Wait for all frames to complete
        void WaitForIdle();

        // Get current frame index (0 to RHI_FRAMES_IN_FLIGHT-1)
        u32 GetCurrentFrameIndex() const;
        // Get absolute frame number
        u64 GetFrameNumber() const;
        // Get time since last frame (in seconds)
        f64 GetDeltaTime() const;
        // Get current FPS
        f64 GetFPS() const;
        // Get frame time in milliseconds
        f64 GetFrameTimeMs() const;

        // Get fence for current frame
        RHIFence* GetCurrentFence();
        const RHIFence* GetCurrentFence() const;

        // Get the fence value to signal for the current frame
        u64 GetNextFenceValue() const;
    };
}
