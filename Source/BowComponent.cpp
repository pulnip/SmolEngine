#include <vector>
#include <utility>

#include "Actor.hpp"
#include "BowComponent.hpp"
#include "Primitives.hpp"
#include "World.hpp"
#include "ArrowActor.hpp"
#include "LogGame.hpp"
#include "SpriteComponent.hpp"

SMOL_COMPONENT(BowComponent)
    .SetProperty("ArrowVelocity", &BowComponent::arrowVelocity)
SMOL_COMPONENT_END(BowComponent)

BowComponent::~BowComponent() = default;

void BowComponent::Update(Smol::f32 dt){

}

void BowComponent::Shoot(Smol::Vec3 direction){
    if (bIsAiming == false){
        return;
    }

    // 화살 생성
    ArrowActor* arrow = owner->GetWorld()->SpawnActor<ArrowActor>();
    if (arrow == nullptr){
        LOG_WARN("nullptr");
        return;
    }

    // 화살이 이동할 궤적
    const int sampleCount = 50;
    std::vector<Smol::Vec3> arrowTrajectory = BuildTrajectory(direction, sampleCount);

    // 계산한 궤적을 화살에게 넘겨서 샘플 위치를 따라 이동시킨다.
    arrow->SetTrajectory(std::move(arrowTrajectory), 0.1f);
}

std::vector<Smol::Vec3> BowComponent::BuildTrajectory(Smol::Vec3 direction, const int sampleCount){
    //TODO.
    // 임시 변수
    Smol::Vec3 startPos = Smol::Vec3(owner->GetTransform().position);
    Smol::Vec3 gravity = Smol::Vec3(0, -9.8f, 0);   // 1unit이 1cm일 경우 -980.f가 적절

    // 계산할 위치 수 및 위치 간격
    const float timeStep = 0.1f;

    std::vector<Smol::Vec3> Points;
    for (int i = 0; i < sampleCount; ++i)
    {
        // 현재 샘플의 시간
        float t = i * timeStep;

        // 위치 = 시작 위치 + 방향 * 초기 속도 * 시간 + 1/2 * 가속도 * 시간^2
        Smol::Vec3 pos = startPos + direction * arrowVelocity * t + 0.5f * gravity * t * t;

        Points.push_back(pos);
    }

    return Points;
}

void BowComponent::SetAiming(bool isAiming){
    bIsAiming = isAiming;
}
