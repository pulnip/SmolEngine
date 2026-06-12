#pragma once

#include "Component.hpp"
#include "Primitives.hpp"

class BowComponent: public Smol::TypedComponent<BowComponent>{
    SMOL_COMPONENT_BODY(BowComponent)

public:
    BowComponent() = default;
    virtual ~BowComponent();
    SMOL_DECLARE_PINNED(BowComponent)

    void Update(Smol::f32) override;

    //TODO. <활 발사>
    void EndAim(Smol::Vec3 direction);

    std::vector<Smol::Vec3> BuildTrajectory(Smol::Vec3 direction);

private:
    Smol::Vec3 arrowVelocity = Smol::Vec3(3);
};
