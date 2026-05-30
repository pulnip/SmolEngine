#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <imgui_impl_sdl3.h>
#include "RuntimeConfig.hpp"
#include "SDLInputProvider.hpp"
#include "Window.hpp"

namespace Smol
{
    class Window::Impl{
    private:
        SDL_Window* window = nullptr;
        u32 width = 0, height = 0;

        SDLInputProvider inputProvider;

    public:
        Impl(const WindowConfig&);
        ~Impl();

        bool ProcessEvents();

        SDL_Window* GetWindow() const noexcept{ return window; }
        u32 GetWidth() const noexcept{ return width; }
        u32 GetHeight() const noexcept{ return height; }

        const SDLInputProvider* GetInputProvider() noexcept{
            return &inputProvider;
        }
    };

    Window::Window(const WindowConfig& config)
        : impl(std::make_unique<Impl>(config)) {}

    Window::Impl::Impl(const WindowConfig& config)
        : inputProvider()
        , width(config.width), height(config.height)
    {
        SDL_InitFlags initFlags = SDL_INIT_VIDEO;
        if(!SDL_InitSubSystem(initFlags)){
            throw std::runtime_error(std::format(
                "Couldn't initialize SDL: {}",
                SDL_GetError()
            ));
        }

        SDL_WindowFlags windowFlags =
            (config.fullscreen    ? SDL_WINDOW_FULLSCREEN    : 0) |
            (config.resizable     ? SDL_WINDOW_RESIZABLE     : 0) |
            (config.borderless    ? SDL_WINDOW_BORDERLESS    : 0) |
            (config.always_on_top ? SDL_WINDOW_ALWAYS_ON_TOP : 0);
        window = SDL_CreateWindow(
            config.title.c_str(),
            config.width,
            config.height,
            windowFlags
        );

        if(window == nullptr){
            SDL_QuitSubSystem(initFlags);

            throw std::runtime_error(std::format(
                "Couldn't create Window: {}",
                SDL_GetError()
            ));
        }
    }

    Window::~Window() = default;

    Window::Impl::~Impl(){
        if(window != nullptr){
            SDL_DestroyWindow(window);
            window = nullptr;

            SDL_QuitSubSystem(SDL_INIT_VIDEO);
        }
    }

    bool Window::ProcessEvents(){
        return impl->ProcessEvents();
    }

    bool Window::Impl::ProcessEvents(){
        bool keepRunning = true;

        inputProvider.NewFrame();

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            // ImGui_ImplSDL3_ProcessEvent(&event);
            switch(event.type){
            case SDL_EVENT_QUIT:
                keepRunning = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                [[fallthrough]];
            case SDL_EVENT_KEY_UP:
                inputProvider.OnPlatformEvent(event);
                break;
            }
        }

        return keepRunning;
    }

    void* Window::GetWindow() const noexcept{ return impl->GetWindow(); }
    u32 Window::GetWidth() const noexcept{ return impl->GetWidth(); }
    u32 Window::GetHeight() const noexcept{ return impl->GetHeight(); }

    const InputProvider* Window::GetInputProvider() const noexcept{
        return impl->GetInputProvider();
    }
}
