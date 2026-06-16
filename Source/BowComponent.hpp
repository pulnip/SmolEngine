#pragma once

#include "Component.hpp"
#include "Primitives.hpp"

class BowActor;

class BowComponent: public Smol::TypedComponent<BowComponent>{
    SMOL_COMPONENT_BODY(BowComponent)

public:
    BowComponent() = default;
    virtual ~BowComponent();
    SMOL_DECLARE_PINNED(BowComponent)

    void Init() override;

    void Update(Smol::f32) override;

    // <활 발사>
    void Shoot(Smol::Vec3 direction);

    std::vector<Smol::Vec3> BuildTrajectory(Smol::Vec3 direction, const int sampleCount = 10);

    void SetBowDirection(Smol::Vec3 direction);

    bool GetIsAiming(){ return bIsAiming; }
    void SetIsAiming(bool isAiming){ bIsAiming = isAiming; }

private:
    float arrowVelocity = 15.f;

    bool bIsAiming = false;

    BowActor* bow;
};
