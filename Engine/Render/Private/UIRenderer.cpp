#include <imgui_impl_sdl3.h>
#include "UIRenderer.hpp"
#include "StringUtil.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHISwapchain.hpp"

namespace Smol
{
    detail::UIRendererInitializer::UIRendererInitializer(
        const std::filesystem::path& contentRoot
    ){
        auto fontPath = contentRoot / "Fonts/NanumBarunGothic.ttf";
        auto utf8Path = toUTF8String(fontPath);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(
            utf8Path.c_str(),
            0.0f,
            nullptr,
            io.Fonts->GetGlyphRangesKorean()
        );
        // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();
    }

    struct UIContext{};
}

#if defined(SMOL_DX11RHI)

#include <imgui_impl_dx11.h>

namespace Smol
{
    UIRenderer::UIRenderer(
        void* sdlWindow,
        RHIDevice& device,
        const std::filesystem::path& contentRoot,
        Widget&& debugWidget
    )
        : initializer(contentRoot)
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

#elif defined(SMOL_METALRHI)

#include <imgui_impl_metal.h>

namespace Smol
{
    UIRenderer::UIRenderer(
        void* sdlWindow,
        RHIDevice& device,
        const std::filesystem::path& contentRoot,
        Widget&& debugWidget
    )
        : initializer(contentRoot)
        , shapeRenderer(device)
        , debugWidget(std::move(debugWidget))
    {
        ImGui_ImplSDL3_InitForMetal(
            static_cast<SDL_Window*>(sdlWindow)
        );
        ImGui_ImplMetal_Init(static_cast<MTL::Device*>(device.Get()));
    }

    UIRenderer::~UIRenderer(){
        ImGui_ImplMetal_Shutdown();

        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void UIRenderer::Draw(RHICommandList& cmdList, RHISwapchain* swapchain){
        auto uiPassDesc = MTL::RenderPassDescriptor::alloc()->init();
        auto colorAttachment = uiPassDesc->colorAttachments()->object(0);
        colorAttachment->setLoadAction(MTL::LoadActionLoad);
        colorAttachment->setStoreAction(MTL::StoreActionStore);
        colorAttachment->setTexture(static_cast<MTL::Texture*>(
            swapchain->GetCurrentNativeTexture()
        ));

        ImGui_ImplMetal_NewFrame(uiPassDesc);
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

        auto commandBuffer = static_cast<MTL::CommandBuffer*>(
            cmdList.GetNative()
        );

        auto uiRenderEncoder = commandBuffer->renderCommandEncoder(uiPassDesc);
        ImGui_ImplMetal_RenderDrawData(
            draw_data,
            commandBuffer,
            uiRenderEncoder
        );
        uiRenderEncoder->endEncoding();

        uiPassDesc->release();
    }
}

#endif
