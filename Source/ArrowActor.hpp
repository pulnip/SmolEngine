#pragma once

#include <span>
#include <vector>

#include "Actor.hpp"

namespace Smol
{
    class ColliderComponent;
    struct OverlapResult2D;
}

class ArrowActor: public Smol::Actor{
    SMOL_ACTOR_BODY(ArrowActor)

public:
    ArrowActor();

    void OnStart() override;

    void OnUpdate(float) override;

    void SetTrajectory(std::vector<Smol::Vec3> points, Smol::f32 pointInterval);
    void ClearTrajectory();

private:
    void BeginTrajectory(Smol::f32 pointInterval);

    void OnOverlapBegin(
       Smol::ColliderComponent* OverlappedComponent,
        Smol::Actor* OtherActor, Smol::ColliderComponent* OtherComp,
        const Smol::OverlapResult2D& Result
    );

    std::vector<Smol::Vec3> trajectoryPoints;
    Smol::f32 trajectoryElapsed = 0.0f;
    Smol::f32 trajectoryPointInterval = 0.1f;
    bool isFollowingTrajectory = false;
};
