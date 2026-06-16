#pragma once

#include "Component.hpp"
#include "Primitives.hpp"
#include <functional>

enum class ElementalType: Smol::u8{
    Fire,
    Water,
};

namespace Smol
{
    class Actor;
}

class ElementalComponent: public Smol::TypedComponent<ElementalComponent>{
    SMOL_COMPONENT_BODY(ElementalComponent)

public:
    ElementalComponent() = default;
    virtual ~ElementalComponent();
    SMOL_DECLARE_PINNED(ElementalComponent)

    void Update(Smol::f32) override;

    void InitProperty(Smol::f32 newCurrentTemperature, Smol::f32 newIgnitionTemperature, Smol::f32 remainingBurnTime = 3.f);

    void ApplyHeat(Smol::f32 Temperature);

    // 연소 시간이 0에 도달하면 true를 반환합니다.
    bool DecreaseBurnTime(Smol::f32 dt);

    Smol::f32 GetCurrentTemperature() { return currentTemperature; }

    std::function<void(Smol::Actor*)> OnFire;

private:
    void TryIgnite(Smol::f32 dt);

private:
    Smol::f32 currentTemperature = 30.f;   // 현재 온도

    Smol::f32 ignitionTemperature = 50.f;  // 불이 붙기 시작하는 온도

    Smol::f32 remainingBurnTime = 3.f;    // 연소 시간
};
