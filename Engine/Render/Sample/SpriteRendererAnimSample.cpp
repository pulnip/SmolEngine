#include <array>
#include "ImageLoader.hpp"
#include "Primitives.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHISwapchain.hpp"
#include "RHITexture.hpp"
#include "RuntimeConfig.hpp"
#include "SpriteRenderer.hpp"
#include "Timer.hpp"
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
    auto proxy = renderer.BindRenderItem(*texture);
    proxy.GetRenderItem().uvScale = {.x = 1.0f/16, .y = 1.0f/16};

    RHIClearColor clearColor{.v = {0.5f, 0.5f, 0.5f, 1.0f}};
    auto cmdList = device->CreateCommandList();

    Timer timer;
    constexpr f32 framePerSeconds = 0.16f;
    constexpr u32 numFrames = 8;

    f32 et = 0.0f;
    u32 animFrame = 0;

    while(true){
        if(!window.ProcessEvents()) [[unlikely]]
            break;

        timer.NewFrame();
        et += timer.GetDeltaTime();
        if(et > framePerSeconds){
            et -= framePerSeconds;
            animFrame = (animFrame + 1) % numFrames;
        }

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

        // In SpriteComponent
        auto& item = proxy.GetRenderItem();
        item.offset = {
            .x = static_cast<f32>(animFrame),
            .y = 0
        };

        renderer.Draw(*cmdList);

        cmdList->EndRenderPass();
        cmdList->Close();

        device->Submit(*cmdList, swapchain.get());
    }

    cmdList->WaitUntilCompleted();

    return 0;
}
