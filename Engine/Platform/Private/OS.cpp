#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#if defined(SMOL_METALRHI)
    #include <SDL3/SDL_metal.h>
#endif
#include <imgui_impl_sdl3.h>
#include "MainLoop.hpp"
#include "OS.hpp"
#include "RuntimeConfig.hpp"

namespace Smol
{
    OS* OS::singleton = nullptr;

    class OS::Impl{
    private:
        SDL_Window* window = nullptr;
    #if defined(SMOL_METALRHI)
        SDL_MetalView view;
    #endif
        u32 width, height;

    public:
        Impl(const RuntimeConfig&);
        ~Impl();

        void Run(MainLoop&);

        u32 GetWidth() const{ return width; }
        u32 GetHeight() const{ return height; }

        NativeWindowHandle GetWindow() const;

    private:
        bool ProcessEvents();

        SDL_Window* createWindow(const RuntimeConfig::WindowConfig&);
    };

    OS::OS(const RuntimeConfig& config){
        singleton = this;

        impl = std::make_unique<Impl>(config);
    }

    OS::Impl::Impl(const RuntimeConfig& config){
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

    void OS::Run(MainLoop& mainLoop){
        impl->Run(mainLoop);
    }

    void OS::Impl::Run(MainLoop& mainLoop){
        if(!mainLoop.Initialize())
            return;

        while(true){
            if(!ProcessEvents())
                break;

            if(!mainLoop.Update())
                break;
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

    SDL_Window* OS::Impl::createWindow(const RuntimeConfig::WindowConfig& config){
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

    u32 OS::GetWidth() const{ return impl->GetWidth(); }
    u32 OS::GetHeight() const{ return impl->GetHeight(); }

    NativeWindowHandle OS::GetWindow() const{
        return impl->GetWindow();
    }

    NativeWindowHandle OS::Impl::GetWindow() const{
    #if defined(_WIN32)
        return SDL_GetPointerProperty(
            SDL_GetWindowProperties(window),
            SDL_PROP_WINDOW_WIN32_HWND_POINTER,
            nullptr
        );
    #elif defined(__APPLE__) && defined(SMOL_METALRHI)
        return SDL_Metal_GetLayer(view);
    #else
        return nullptr;
    #endif
    }
}