#include "Actor.hpp"
#include "LineRenderer.hpp"
#include "ShapeRenderer.hpp"
#include "World.hpp"

namespace Smol
{
    SMOL_COMPONENT(LineRenderer)
    SMOL_COMPONENT_END(LineRenderer)

    void LineRenderer::DrawLine(std::span<const Vec3> points, Color color){
        auto world = owner->GetWorld();
        auto renderer = world->GetShapeRenderer();

        renderer->SubmitLine(points, color);
    }

    void LineRenderer::DrawText(StrView text, Vec2 pos, Color color, f32 fontSize){
        auto world = owner->GetWorld();
        auto renderer = world->GetShapeRenderer();

        renderer->SubmitText(text, pos, color, fontSize);
    }
}
