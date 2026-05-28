#include "RHIDefinitions.hpp"
#include "RHIDevice.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#if defined(SMOL_METALRHI)
    #include <SDL3/SDL_metal.h>
#endif
#include <imgui_impl_sdl3.h>
#include "CommandListPool.hpp"
#include "FramePacer.hpp"
#include "MainLoop.hpp"
#include "OS.hpp"
#include "RHISwapchain.hpp"
#include "RuntimeConfig.hpp"
#include "Timer.hpp"

namespace Smol
{
    OS* OS::singleton = nullptr;

    class OS::Impl{
    private:
        SDL_Window* window = nullptr;
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
        ~Impl();

        void Run(MainLoop&, RHIDevice&);

        u32 GetWidth() const{ return width; }
        u32 GetHeight() const{ return height; }

    private:
        bool ProcessEvents();

        SDL_Window* createWindow(const WindowConfig&);

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
        : framePacer(device)
        , cmdListPool(device)
    {
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

        SDL_InitFlags flags = SDL_INIT_EVENTS | SDL_INIT_VIDEO;
        if(!SDL_Init(flags)){
            throw std::runtime_error(std::format(
                "Couldn't initialize SDL: {}",
                SDL_GetError()
            ));
        }

        window = createWindow(config.window);
        if(window == nullptr){
            throw std::runtime_error(std::format(
                "Couldn't create Window: {}",
                SDL_GetError()
            ));
        }

    #if defined(SMOL_METALRHI)
        view = SDL_Metal_CreateView(window);
    #endif

        RHITextureCreateDesc backBufferDesc{
            .width = config.window.width,
            .height = config.window.height,
            .format = RHIPixelFormat::RGBA8_UNORM
        };

        swapchain = device.CreateSwapchain(RHISwapchainCreateDesc{
        #if defined(SMOL_DX11RHI)
            .windowHandle = SDL_GetPointerProperty(
                SDL_GetWindowProperties(window),
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

    OS::Impl::~Impl(){
        if(window != nullptr){
            SDL_DestroyWindow(window);
            window = nullptr;
        }
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

            [[unlikely]] if(!ProcessEvents())
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

    bool OS::Impl::ProcessEvents(){
        bool keepRunning = true;

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            // ImGui_ImplSDL3_ProcessEvent(&event);
            switch(event.type){
            case SDL_EVENT_QUIT:
                keepRunning = false;
                break;
            }
        }

        return keepRunning;
    }

    SDL_Window* OS::Impl::createWindow(const WindowConfig& config){
        width = config.width;
        height = config.height;

        SDL_WindowFlags flags =
            (config.fullscreen    ? SDL_WINDOW_FULLSCREEN    : 0) |
            (config.resizable     ? SDL_WINDOW_RESIZABLE     : 0) |
            (config.borderless    ? SDL_WINDOW_BORDERLESS    : 0) |
            (config.always_on_top ? SDL_WINDOW_ALWAYS_ON_TOP : 0);

        return SDL_CreateWindow(
            config.title.c_str(),
            config.width,
            config.height,
            flags
        );
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

    u32 OS::GetWidth() const{ return impl->GetWidth(); }
    u32 OS::GetHeight() const{ return impl->GetHeight(); }
}
