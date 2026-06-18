#include <imgui_impl_sdl3.h>
#include "UIRenderer.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHISwapchain.hpp"

#if defined(SMOL_DX11RHI)

#include <imgui_impl_dx11.h>

namespace Smol
{
    detail::UIRendererInitializer::UIRendererInitializer(){
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();
    }

    struct UIContext{};

    UIRenderer::UIRenderer(
        void* sdlWindow,
        RHIDevice& device,
        Widget&& debugWidget
    )
        : initializer()
        , shapeRenderer(device)
        , debugWidget(std::move(debugWidget))
    {
        ImGui_ImplSDL3_InitForD3D(
            static_cast<SDL_Window*>(sdlWindow)
        );
        ImGui_ImplDX11_Init(
            static_cast<ID3D11Device*>(
                device.Get()
            ),
            static_cast<ID3D11DeviceContext*>(
                device.GetContextOrQueue()
            )
        );
    }

    UIRenderer::~UIRenderer(){
        ImGui_ImplDX11_Shutdown();

        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void UIRenderer::Draw(RHICommandList& cmdList, RHISwapchain* swapchain){
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        shapeRenderer.Draw(*swapchain);

        UIContext uiContext{};
        ImGui::Begin("Debug");
        std::visit([&uiContext](auto& widget){
            widget.submit(uiContext);
        }, debugWidget);
        ImGui::End();

        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();

        ImGui_ImplDX11_RenderDrawData(draw_data);
    }
}

#endif
