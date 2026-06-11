#include "CameraConfig.hpp"
#include "Canvas2D.hpp"
#include "LineProxy.hpp"
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

    void fillLine2D(
        const std::span<const Smol::Vec3> line3d,
        std::vector<Smol::Vec2>& line2d
    ){
        using namespace Smol;

        line2d.clear();
        line2d.reserve(line3d.size());

        for(const auto& point: line3d){
            line2d.push_back(worldToScreen(point));
        }
    }
}

namespace Smol
{
    ShapeRenderer::ShapeRenderer(RHIDevice& device)
        : canvas(Smol::CreateCanvas(device))
    {}

    ShapeRenderer::~ShapeRenderer() = default;

    LineProxy ShapeRenderer::BindLine(std::span<const Vec3> points, Color color){
        auto handle = lines.Emplace(points, color);

        return LineProxy(handle, *this);
    }

    void ShapeRenderer::Draw(RHISwapchain& swapchain){
        canvas->Begin(swapchain);

        std::vector<Vec2> line2d;

        for(const auto& item: lines){
            fillLine2D(item.points, line2d);

            canvas->Polyline(line2d, item.style);
        }

        canvas->End();
    }

    LineRenderItem& ShapeRenderer::Get(LineHandle handle){
        return lines.GetRef(handle);
    }

    void ShapeRenderer::Unbind(LineHandle handle){
        lines.Remove(handle);
    }
}
