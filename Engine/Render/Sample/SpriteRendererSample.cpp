#include <array>
#include "ImageLoader.hpp"
#include "Primitives.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHISwapchain.hpp"
#include "RHITexture.hpp"
#include "RuntimeConfig.hpp"
#include "SpriteRenderer.hpp"
#include "Window.hpp"

using namespace Smol;

CStr IMAGE_PATH = "Content/Assets/Sprite/hollow_knight.png";

int main(void){
    auto device = CreateDevice();
    WindowConfig windowConfig{
        .title = "TextureSample",
        .width = 800, .height = 800,
        .fullscreen = false,
        .resizable = false,
    };
    Window window(windowConfig);
    auto fWidth = static_cast<f32>(window.GetWidth());
    auto fHeight = static_cast<f32>(window.GetHeight());

    RHITextureCreateDesc backBufferDesc{
        .width = windowConfig.width, .height = windowConfig.height,
        .format = RHIPixelFormat::RGBA8_UNORM
    };
    auto swapchain = device->CreateSwapchain(RHISwapchainCreateDesc{
        .sdlWindow = window.GetWindow(),
        .bufferDesc = backBufferDesc
    });

    SpriteRenderer renderer(*device);

    // load Texture from Image
    auto image = loadImage(IMAGE_PATH);
    auto texture = device->CreateTexture(RHITextureCreateDesc{
        .width = image.GetWidth(), .height = image.GetHeight(),
        .format = RHIPixelFormat::RGBA8_UNORM,
        .usage = RHITextureUsage::AllowShaderRead,
        .initialData = image.GetBufferPointer()
    });
    std::array items = {
        SpriteRenderItem{
            .uvScale = {1.0f/16, 1.0f/16},
            .offset = {0, 0},
            .texture = *texture
        }
    };
    auto _ = renderer.BindRenderItem(*texture);

    RHIClearColor clearColor{.v = {0.5f, 0.5f, 0.5f, 1.0f}};
    auto cmdList = device->CreateCommandList();

    while(true){
        if(!window.ProcessEvents()) [[unlikely]]
            break;

        cmdList->Begin();
        cmdList->BeginRenderPass(*swapchain,
            clearColor,
            nullptr,
            {},
            RHILoadAction::Clear
        );

        cmdList->SetViewport(RHIViewport{
            .x = 0, .y = 0,
            .width = fWidth, .height = fHeight,
            .minDepth = 0, .maxDepth = 1
        });

        renderer.Draw(*cmdList);

        cmdList->EndRenderPass();
        cmdList->Close();

        device->Submit(*cmdList, swapchain.get());
    }

    cmdList->WaitUntilCompleted();

    return 0;
}
