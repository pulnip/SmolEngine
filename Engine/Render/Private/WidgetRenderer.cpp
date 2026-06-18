#include "Widget.hpp"
#include <imgui_impl_sdl3.h>
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHISwapchain.hpp"
#include "WidgetRenderer.hpp"

#if defined(SMOL_DX11RHI)

#include <imgui_impl_dx11.h>

namespace Smol
{
    class WidgetRenderer::Impl{
    public:
        Impl(SDL_Window* window,
            ID3D11Device* device,
            ID3D11DeviceContext* context
        );

        ~Impl();

        void Draw(
            StrView uiName,
            Widget& ui,
            UIContext& uiCtx,
            RHICommandList& cmdList,
            RHISwapchain* backBuffer
        );
    };

    WidgetRenderer::WidgetRenderer(void* sdlWindow, RHIDevice& device)
        :impl(std::make_unique<Impl>(
            static_cast<SDL_Window*>(sdlWindow),
            static_cast<ID3D11Device*>(device.Get()),
            static_cast<ID3D11DeviceContext*>(device.GetContextOrQueue())
        ))
    {}

    WidgetRenderer::Impl::Impl(SDL_Window* window,
        ID3D11Device* device,
        ID3D11DeviceContext* context
    ){
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        ImGui_ImplSDL3_InitForD3D(window);
        ImGui_ImplDX11_Init(device, context);
    }

    WidgetRenderer::Impl::~Impl(){
        ImGui_ImplDX11_Shutdown();

        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void WidgetRenderer::Impl::Draw(
        StrView uiName,
        Widget& ui,
        UIContext& uiCtx,
        RHICommandList&,
        RHISwapchain*
    ){
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin(uiName.data());
        std::visit([&uiCtx](auto& widget){
            widget.submit(uiCtx);
        }, ui);
        ImGui::End();

        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();

        ImGui_ImplDX11_RenderDrawData(draw_data);
    }
}

#endif

namespace Smol
{
    WidgetRenderer::~WidgetRenderer() = default;

    void WidgetRenderer::Draw(
        StrView uiName,
        Widget& ui,
        UIContext& uiCtx,
        RHICommandList& cmdList,
        RHISwapchain* backBuffer
    ){
        impl->Draw(uiName, ui, uiCtx, cmdList, backBuffer);
    }
}
