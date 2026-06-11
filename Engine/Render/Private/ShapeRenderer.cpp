#include "CameraConfig.hpp"
#include "Canvas2D.hpp"
#include "ShapeRenderer.hpp"

namespace
{
    Smol::Vec2 worldToScreen(Smol::Vec3 world){
        using namespace Smol;

        const auto viewPos = view * world;

        const auto p = viewToScreen * viewPos;

        return Vec2{
            0.5f * screenWidth + viewPos.x * viewToScreen,
            0.5f * screenHeight - viewPos.y * viewToScreen,
        };
    }
}

namespace Smol
{
    ShapeRenderer::ShapeRenderer(RHIDevice& device)
        : canvas(Smol::CreateCanvas(device))
    {}

    ShapeRenderer::~ShapeRenderer() = default;

    void ShapeRenderer::Draw(RHISwapchain& swapchain){
        canvas->Begin(swapchain);

        // TODO.
        auto a = worldToScreen(Vec3{0, 10, 0});
        auto b = worldToScreen(Vec3{15, 5, 0});

        canvas->Line(a, b, StrokeStyle{
            .width = 1
        });

        canvas->End();
    }
}
