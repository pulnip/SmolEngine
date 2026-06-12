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

    void ApplyHeat(Smol::f32 Temperature);
    // 연소 시간이 0에 도달하면 true를 반환합니다.
    bool DecreaseBurnTime(Smol::f32 dt);

    std::function<void(Smol::Actor*)> OnFire;

    Smol::f32 GetCurrentTemperature() { return CurrentTemperature; }

private:
    void TryIgnite(Smol::f32 dt);

private:
    Smol::f32 CurrentTemperature = 30.f;   // 현재 온도

    Smol::f32 IgnitionTemperature = 10.f;  // 불이 붙기 시작하는 온도

    Smol::f32 remainingBurnTime = 10.f;    // 연소 시간
};
