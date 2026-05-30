#include <SDL3/SDL_init.h>
#include <SDL3/SDL_error.h>
#include "CommandListPool.hpp"
#include "FramePacer.hpp"
#include "MainLoop.hpp"
#include "OS.hpp"
#include "RHIDefinitions.hpp"
#include "RHIDevice.hpp"
#include "RHISwapchain.hpp"
#include "RuntimeConfig.hpp"
#include "Timer.hpp"
#include "Window.hpp"

namespace Smol
{
    OS* OS::singleton = nullptr;

    class SDLInitializer final{
    public:
        SDLInitializer(const RuntimeConfig&);
        ~SDLInitializer() = default;
    };

    SDLInitializer::SDLInitializer(const RuntimeConfig& config){
        if(!SDL_SetAppMetadata(
            config.name.c_str(),
            config.version.c_str(),
            config.identifier.c_str()
        )){
            throw std::runtime_error(std::format(
                "Couldn't set runtime metadata: {}",
                SDL_GetError()
            ));
        }
    }

    class OS::Impl{
    private:
        SDLInitializer initializer;
        Window window;
        RHISwapchainRAII swapchain = nullptr;
        u32 width = 0, height = 0;

        // Non-stop timer
        Timer sysTimer;
        FramePacer framePacer;
        CommandListPool cmdListPool;

    public:
        Impl(const RuntimeConfig&, RHIDevice&);
        ~Impl() = default;

        void Run(MainLoop&, RHIDevice&);

        const InputProvider* GetInputProvider() noexcept{
            return window.GetInputProvider();
        }

    private:
        void BeginFrame(RHIDevice&);
        void EndFrame(RHIDevice&);
    };

    OS::OS(
        const RuntimeConfig& config,
        RHIDevice& device
    )
        : impl(std::make_unique<Impl>(config, device))
    {
        singleton = this;
    }

    OS::Impl::Impl(
        const RuntimeConfig& config,
        RHIDevice& device
    )
        : initializer(config)
        , window(config.window)
        , framePacer(device)
        , cmdListPool(device)
    {
        RHITextureCreateDesc backBufferDesc{
            .width = config.window.width,
            .height = config.window.height,
            .format = RHIPixelFormat::RGBA8_UNORM
        };

        swapchain = device.CreateSwapchain(RHISwapchainCreateDesc{
            .sdlWindow = window.GetWindow(),
            .bufferDesc = backBufferDesc,
            // triple buffering
            .bufferCount = 3,
            .vsync = true,
            .allowTearing = false
        #if defined(_DEBUG) || !defined(NDEBUG)
            , .debugName = std::format("Swapchain for {}", config.window.title)
        #endif
        });
    }

    OS::~OS(){
        singleton = nullptr;
    }

    void OS::Run(MainLoop& mainLoop, RHIDevice& device){
        impl->Run(mainLoop, device);
    }

    void OS::Impl::Run(MainLoop& mainLoop, RHIDevice& device){
        if(!mainLoop.Initialize())
            return;

        sysTimer.Reset();

        while(true){
            sysTimer.NewFrame();

            if(!window.ProcessEvents()) [[unlikely]]
                break;

            if(!mainLoop.Update()) [[unlikely]]
                break;

            BeginFrame(device);
            if(!mainLoop.Render(cmdListPool)) [[unlikely]]
                break;
            EndFrame(device);
        }

        mainLoop.Finalize();
    }

    void OS::Impl::BeginFrame(RHIDevice& device){
        framePacer.BeginFrame();

        if(swapchain != nullptr)
            swapchain->AcquireNextImage();

        cmdListPool.BeginFrame();
    }

    void OS::Impl::EndFrame(RHIDevice& device){
        cmdListPool.SubmitFrame(
            swapchain.get(),
            framePacer.GetCurrentFence(),
            framePacer.GetNextFenceValue()
        );

        framePacer.EndFrame();
    }

    const InputProvider* OS::GetInputProvider() const noexcept{
        return impl->GetInputProvider();
    }
}
