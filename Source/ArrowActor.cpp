#include <cmath>
#include <utility>

#include "ArrowActor.hpp"
#include "ColliderComponent.hpp"
#include "ElementalComponent.hpp"
#include "FireActor.hpp"
#include "LinearAlgebra.hpp"
#include "LogGame.hpp"
#include "SpriteComponent.hpp"
#include "World.hpp"

namespace
{
    // ArrowActor에서 쓰는 튜닝 값은 한 곳에 모아 런타임 로직을 가볍게 유지한다.
    constexpr char ArrowSpriteName[] = "Basic_Arrow";

    constexpr Smol::f32 DefaultTrajectoryPointInterval = 0.1f;
    constexpr Smol::f32 MinMoveDirectionLengthSquared = 0.0001f;

    constexpr Smol::f32 ArrowDepth = 1.0f;
    constexpr Smol::Vec2 ArrowScale = {2.0f, 2.0f};

    constexpr Smol::Vec2 ArrowColliderScale = {0.5f, 0.3f};
    constexpr Smol::Vec2 FireScale = {0.3f, 0.3f};

    constexpr Smol::f32 FireHeight = 0.2f;
    constexpr Smol::f32 FireDepth = 0.0f;

    constexpr Smol::f32 ArrowCurrentTemperature = 30.0f;
    constexpr Smol::f32 ArrowIgnitionTemperature = 50.0f;

    void RotateToMoveDirection(Smol::Transform& transform, Smol::Vec3 moveDelta)
    {
        const Smol::f32 xyLengthSquared =
            moveDelta.x * moveDelta.x + moveDelta.y * moveDelta.y;
        if (xyLengthSquared <= MinMoveDirectionLengthSquared)
        {
            return;
        }

        // 화살 스프라이트의 +X 방향이 이동 방향을 바라보도록 Z축 회전만 적용한다.
        transform.rotation = Smol::rotateZ(std::atan2(moveDelta.y, moveDelta.x));
    }
}

SMOL_ACTOR(ArrowActor, Smol::Actor)
SMOL_ACTOR_END(ArrowActor)

ArrowActor::ArrowActor() = default;

void ArrowActor::OnStart(){
    Smol::SpriteComponent* spriteComp = AddComponent<Smol::SpriteComponent>();
    spriteComp->OnAttach(ArrowSpriteName);

    Smol::ColliderComponent* colliderComp = AddComponent<Smol::ColliderComponent>();
    colliderComp->OnAttach();
    colliderComp->SetLayer(0b1);
    colliderComp->SetMask(0b0);
    colliderComp->SetScale(ArrowColliderScale);

    ElementalComponent* elementalComp = AddComponent<ElementalComponent>();
    elementalComp->OnFire = [this](Smol::Actor*) {
        Ignite();
    };
    elementalComp->InitProperty(ArrowCurrentTemperature, ArrowIgnitionTemperature);

    auto& transform = GetTransform();
    transform.position.z = ArrowDepth;
    transform.scale = ArrowScale;
}

void ArrowActor::OnUpdate(float dt){
    if (!isFollowingTrajectory)
    {
        return;
    }

    UpdateTrajectory(dt);
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
    trajectoryPointInterval = pointInterval > 0.0f
        ? pointInterval
        : DefaultTrajectoryPointInterval;
    isFollowingTrajectory = !trajectoryPoints.empty();

    if (isFollowingTrajectory)
    {
        // 궤적 이동은 항상 첫 샘플 위치에서 시작한다.
        GetTransform().position = trajectoryPoints.front();
    }
}

void ArrowActor::UpdateTrajectory(Smol::f32 dt){
    if (trajectoryPoints.empty())
    {
        ClearTrajectory();
        return;
    }

    trajectoryElapsed += dt;

    const Smol::usize lastSegmentIndex = trajectoryPoints.size() - 1;
    const Smol::f32 totalDuration =
        static_cast<Smol::f32>(lastSegmentIndex) * trajectoryPointInterval;
    if (lastSegmentIndex == 0 || trajectoryElapsed >= totalDuration)
    {
        // 마지막 샘플을 넘어가면 보간하지 않고 끝 위치에 고정한다.
        FinishTrajectory();
        return;
    }

    const Smol::f32 progress = trajectoryElapsed / trajectoryPointInterval;
    const Smol::usize segmentIndex = static_cast<Smol::usize>(progress);
    const Smol::f32 alpha = progress - static_cast<Smol::f32>(segmentIndex);

    const Smol::Vec3 start = trajectoryPoints[segmentIndex];
    const Smol::Vec3 end = trajectoryPoints[segmentIndex + 1];
    const Smol::Vec3 moveDelta = end - start;

    // 현재 샘플과 다음 샘플 사이를 보간해 프레임마다 부드럽게 이동한다.
    auto& transform = GetTransform();
    RotateToMoveDirection(transform, moveDelta);
    transform.position = start + moveDelta * alpha;
}

void ArrowActor::FinishTrajectory(){
    auto& transform = GetTransform();
    if (trajectoryPoints.size() >= 2)
    {
        // 도착 후에도 마지막 진행 방향을 유지해 화살의 방향이 튀지 않게 한다.
        const Smol::Vec3 moveDelta =
            trajectoryPoints.back() - trajectoryPoints[trajectoryPoints.size() - 2];
        RotateToMoveDirection(transform, moveDelta);
    }

    transform.position = trajectoryPoints.back();
    ClearTrajectory();
}

void ArrowActor::Ignite(){
    FireActor* fire = GetWorld()->SpawnActor<FireActor>();
    if (fire == nullptr)
    {
        LOG_WARN("Failed to spawn FireActor");
        return;
    }

    ElementalComponent* elementalComp = GetComponent<ElementalComponent>();
    if (elementalComp != nullptr)
    {
        // 이미 불이 붙은 화살은 같은 FireActor를 반복 생성하지 않는다.
        elementalComp->OnFire = nullptr;
    }

    fire->AttachTo(this, false);

    auto& fireTransform = fire->GetTransform();
    fireTransform.position.y = FireHeight;
    fireTransform.position.z = FireDepth;
    fireTransform.scale = FireScale;
}
