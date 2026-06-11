#include "Actor.hpp"
#include "LineRenderer.hpp"
#include "ShapeRenderer.hpp"
#include "World.hpp"

namespace Smol
{
    SMOL_COMPONENT(LineRenderer)
    SMOL_COMPONENT_END(LineRenderer)

    void LineRenderer::Update(f32){
        proxies.clear();
    }

    void LineRenderer::DrawLine(std::span<const Vec3> points, Color color){
        auto world = owner->GetWorld();
        auto renderer = world->GetShapeRenderer();

        auto proxy = renderer->BindLine(points, color);
        proxies.emplace_back(std::move(proxy));
    }
}
