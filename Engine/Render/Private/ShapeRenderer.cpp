#include "CameraConfig.hpp"
#include "Canvas2D.hpp"
#include "ShapeRenderer.hpp"

namespace
{
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

    void ShapeRenderer::SubmitLine(std::span<const Vec3> points, Color color){
        lines.emplace_back(points, color);
    }

    void ShapeRenderer::SubmitOBB2D(OBB2D obb, Color color){
        Vec3 c = obb.center;
        Vec3 x = obb.halfAxes[0];
        Vec3 y = obb.halfAxes[1];

        std::array line{
            // Bottom - Left
            c - x - y,
            // Top - Left
            c - x + y,
            // Top - Right
            c + x + y,
            // Bottom - Right
            c + x - y,
            // Closed form
            c - x - y
        };

        lines.emplace_back(line, color);
    }

    void ShapeRenderer::Draw(RHISwapchain& swapchain){
        canvas->Begin(swapchain);

        std::vector<Vec2> line2d;

        for(const auto& item: lines){
            fillLine2D(item.points, line2d);

            canvas->Polyline(line2d, item.style);
        }

        canvas->End();
        lines.clear();
    }
}
