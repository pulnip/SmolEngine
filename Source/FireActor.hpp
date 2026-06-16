#pragma once

#include "Actor.hpp"
#include <vector>

namespace Smol
{
    class ColliderComponent;
    struct OverlapResult2D;
}

class ElementalComponent;

class FireActor: public Smol::Actor{
    SMOL_ACTOR_BODY(FireActor)

public:
    FireActor();

    void OnStart() override;

    void OnUpdate(float) override;

private:
    void OnOverlapBegin(
        Smol::ColliderComponent* overlappedComponent,
        Smol::Actor* otherActor, Smol::ColliderComponent* otherComp,
        const Smol::OverlapResult2D& result
    );

    void OnOverlapEnd(
    Smol::ColliderComponent* overlappedComponent,
    Smol::Actor* otherActor, Smol::ColliderComponent* otherComp);

    // 현재 불과 오버랩 중인 ElementalComponent 목록
    std::vector<ElementalComponent*> overlappingElementalComps;
};
