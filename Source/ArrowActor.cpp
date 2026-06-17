#include <cmath>
#include <utility>

#include "ArrowActor.hpp"
#include "SpriteAnimComponent.hpp"
#include "SpriteComponent.hpp"
#include "ColliderComponent.hpp"
#include "ElementalComponent.hpp"
#include "LinearAlgebra.hpp"
#include "LogGame.hpp"
#include "World.hpp"
#include "FireActor.hpp"

namespace
{
    void RotateToMoveDirection(Smol::Transform& transform, Smol::Vec3 direction)
    {
        const Smol::f32 xyLengthSquared = direction.x * direction.x + direction.y * direction.y;
        if (xyLengthSquared <= 0.0001f)
            return;

        // 화살 스프라이트의 오른쪽(+X)이 이동 방향을 바라보도록 Z축 회전한다.
        transform.rotation = Smol::rotateZ(std::atan2(direction.y, direction.x));
    }
}

SMOL_ACTOR(ArrowActor, Smol::Actor)
SMOL_ACTOR_END(ArrowActor)

ArrowActor::ArrowActor()
{

}

void ArrowActor::OnStart(){
    AddComponent<Smol::SpriteComponent>();
    Smol::SpriteComponent* spriteComp = GetComponent<Smol::SpriteComponent>();
    spriteComp->OnAttach("Basic_Arrow");

    AddComponent<Smol::ColliderComponent>()->OnAttach();
    Smol::ColliderComponent* colliderComp = GetComponent<Smol::ColliderComponent>();
    colliderComp->OnBeginOverlap(this, &ArrowActor::OnOverlapBegin);

    colliderComp->SetLayer(0b1);
    colliderComp->SetMask(0b0);

    Smol::Vec2 colliderScale = Smol::Vec2(0.5f, 0.3f);
    colliderComp->SetScale(colliderScale);

    AddComponent<ElementalComponent>();

    ElementalComponent* elementalComp = GetComponent<ElementalComponent>();
    elementalComp->OnFire = [this](Actor* IgnitedActor){
        // 붙어 있는 불이 없을 경우
        FireActor* fire = GetWorld()->SpawnActor<FireActor>();
        if (fire == nullptr){
            LOG_WARN("", "nullptr");
            return;
        }

        // 더이상 불을 생성하지 않도록 한다
        ElementalComponent* elementalComp = GetComponent<ElementalComponent>();
        elementalComp->OnFire = nullptr;

        fire->AttachTo(this, false);
        fire->GetTransform().position.y = 0.2f;
        fire->GetTransform().position.z = 0.f;
        fire->GetTransform().scale = Smol::Vec2(0.3f, 0.3f);
    };
    elementalComp->InitProperty(30.f, 50.f);

    GetTransform().position.z = 1.f;
    GetTransform().scale = Smol::Vec2(2.f, 2.f);
}

void ArrowActor::OnUpdate(float dt){
    if (!isFollowingTrajectory){
        return;
    }

    if (trajectoryPoints.empty()){
        ClearTrajectory();
        return;
    }

    trajectoryElapsed += dt;

    const Smol::f32 totalDuration =
        static_cast<Smol::f32>(trajectoryPoints.size() - 1) * trajectoryPointInterval;

    // 궤적 끝에 도착하면 마지막 위치에 고정하고 이동을 종료한다.
    if (trajectoryPoints.size() == 1 || trajectoryElapsed >= totalDuration){
        auto& transform = GetTransform();
        if (trajectoryPoints.size() >= 2){
            RotateToMoveDirection(
                transform,
                trajectoryPoints.back() - trajectoryPoints[trajectoryPoints.size() - 2]
            );
        }

        transform.position = trajectoryPoints.back();
        ClearTrajectory();
        return;
    }

    const Smol::f32 progress = trajectoryElapsed / trajectoryPointInterval;
    const Smol::usize segmentIndex = static_cast<Smol::usize>(progress);
    const Smol::f32 alpha = progress - static_cast<Smol::f32>(segmentIndex);

    const Smol::Vec3 start = trajectoryPoints[segmentIndex];
    const Smol::Vec3 end = trajectoryPoints[segmentIndex + 1];
    auto& transform = GetTransform();

    // 현재 샘플과 다음 샘플 사이를 보간해서 부드럽게 이동한다.
    RotateToMoveDirection(transform, end - start);
    transform.position = start + (end - start) * alpha;
}

void ArrowActor::SetTrajectory(std::vector<Smol::Vec3> points, Smol::f32 pointInterval){
    trajectoryPoints = std::move(points);
    BeginTrajectory(pointInterval);

}

void ArrowActor::ClearTrajectory(){
    trajectoryPoints.clear();
    trajectoryElapsed = 0.0f;
    isFollowingTrajectory = false;

}

void ArrowActor::BeginTrajectory(Smol::f32 pointInterval){
    trajectoryElapsed = 0.0f;
    trajectoryPointInterval = pointInterval > 0.0f ? pointInterval : 0.1f;
    isFollowingTrajectory = !trajectoryPoints.empty();

    // 궤적이 시작되면 화살을 첫 번째 위치로 배치한다.
    if (isFollowingTrajectory)
        GetTransform().position = trajectoryPoints.front();

}

void ArrowActor::OnOverlapBegin(Smol::ColliderComponent*, Smol::Actor*, Smol::ColliderComponent*, const Smol::OverlapResult2D& result){
    // LOG_INFO(" Overlap Start. {}", result.contactPoint);

    ElementalComponent* elementalComponent = GetComponent<ElementalComponent>();
    if (nullptr == elementalComponent)
    {
        LOG_WARN("nullptr");
        return;
    }


}
