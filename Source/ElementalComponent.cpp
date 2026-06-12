#include "ElementalComponent.hpp"
#include "LogGame.hpp"
#include "World.hpp"
#include "FireActor.hpp"

SMOL_COMPONENT(ElementalComponent)
    .SetProperty("IgnitionTemperature", &ElementalComponent::IgnitionTemperature)
    .SetProperty("BurnTime", &ElementalComponent::remainingBurnTime)
SMOL_COMPONENT_END(ElementalComponent)

ElementalComponent::~ElementalComponent() = default;

void ElementalComponent::Update(Smol::f32 dt){
    // 현재 불이 붙어야하는 조건일 경우
    if (CurrentTemperature >= IgnitionTemperature){
        TryIgnite(dt);
    }
}

void ElementalComponent::TryIgnite(Smol::f32 dt){
    if (OnFire)
    {
        OnFire(owner);
    }
}

void ElementalComponent::ApplyHeat(Smol::f32 Temperature)
{
    CurrentTemperature = Temperature;
}

bool ElementalComponent::DecreaseBurnTime(Smol::f32 dt)
{
    remainingBurnTime -= dt;
    return remainingBurnTime <= 0.0f;
}
