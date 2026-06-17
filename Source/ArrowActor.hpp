#pragma once

#include <vector>

#include "Actor.hpp"

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
    void UpdateTrajectory(Smol::f32 dt);
    void FinishTrajectory();
    void Ignite();

    std::vector<Smol::Vec3> trajectoryPoints;
    Smol::f32 trajectoryElapsed = 0.0f;
    Smol::f32 trajectoryPointInterval = 0.1f;
    bool isFollowingTrajectory = false;
};
