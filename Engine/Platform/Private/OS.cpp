#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#if defined(SMOL_METALRHI)
    #include <SDL3/SDL_metal.h>
#endif
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
    #if defined(SMOL_METALRHI)
        SDL_MetalView view;
    #endif
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
    #if defined(SMOL_METALRHI)
        view = SDL_Metal_CreateView(window);
    #endif

        RHITextureCreateDesc backBufferDesc{
            .width = config.window.width,
            .height = config.window.height,
            .format = RHIPixelFormat::RGBA8_UNORM
        };

        auto sdlWindow = static_cast<SDL_Window*>(window.GetWindow());
        swapchain = device.CreateSwapchain(RHISwapchainCreateDesc{
        #if defined(SMOL_DX11RHI)
            .windowHandle = SDL_GetPointerProperty(
                SDL_GetWindowProperties(sdlWindow),
                SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                nullptr
            ),
        #elif defined(SMOL_METALRHI)
            .windowHandle = SDL_Metal_GetLayer(view),
        #endif
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

            [[unlikely]] if(!window.ProcessEvents())
                break;

            [[unlikely]] if(!mainLoop.Update())
                break;

            BeginFrame(device);
            [[unlikely]] if(!mainLoop.Render(cmdListPool))
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
