#include <algorithm>

#include "FireActor.hpp"
#include "ElementalComponent.hpp"
#include "ColliderComponent.hpp"
#include "SpriteAnimComponent.hpp"
#include "SpriteComponent.hpp"
#include "LogGame.hpp"

SMOL_ACTOR(FireActor)
SMOL_ACTOR_END(FireActor)

FireActor::FireActor(){
    //AddComponent<Smol::SpriteComponent>();
}

void FireActor::OnStart(){
    AddComponent<Smol::SpriteAnimComponent>();
    Smol::SpriteAnimComponent* spriteAnimComp = GetComponent<Smol::SpriteAnimComponent>();
    spriteAnimComp->OnAttach("Fire");
    Smol::Vec2 spriteScale = Smol::Vec2(5, 5);
    spriteAnimComp->SetSpriteScale(spriteScale);

    AddComponent<Smol::ColliderComponent>()->OnAttach();

    Smol::ColliderComponent* colliderComp = GetComponent<Smol::ColliderComponent>();
    if (nullptr == colliderComp)
    {
        return;
    }
    colliderComp->OnBeginOverlap(this, &FireActor::OnOverlapBegin);
    colliderComp->OnEndOverlap(this, &FireActor::OnOverlapEnd);

    colliderComp->SetLayer(0b1);
    colliderComp->SetMask(0b1);

    AddComponent<ElementalComponent>();
    ElementalComponent* elementalComp = GetComponent<ElementalComponent>();
    if (nullptr == elementalComp)
    {
        return;
    }
    elementalComp->InitProperty(100.f, 10000.f);
}

void FireActor::OnUpdate(float dt){
    // 불과 오버랩 중인 대상의 연소 시간을 감소시킨다
    for (auto it = overlappingElementalComps.begin(); it != overlappingElementalComps.end();)
    {
        ElementalComponent* otherElementalComp = *it;
        if (nullptr == otherElementalComp)
        {
            it = overlappingElementalComps.erase(it);
            continue;
        }

        if (otherElementalComp->DecreaseBurnTime(dt))
        {
            Smol::Actor* otherActor = otherElementalComp->GetOwner();
            it = overlappingElementalComps.erase(it);

            if (nullptr != otherActor)
            {
                otherActor->Destroy();
            }
            continue;
        }

        ++it;
    }
}

void FireActor::OnOverlapBegin(Smol::ColliderComponent* overlappedComponent,
    Smol::Actor* otherActor, Smol::ColliderComponent* otherComp,
    const Smol::OverlapResult2D& result){
    LOG_INFO(" Overlap Start. {}", otherActor->GetClassName());

    ElementalComponent* curElementalComp = GetComponent<ElementalComponent>();
    if (nullptr == curElementalComp)
    {
        LOG_WARN("nullptr");
        return;
    }

    ElementalComponent* otherElementalComp = otherActor->GetComponent<ElementalComponent>();
    if (nullptr == otherElementalComp)
    {
        return;
    }

    otherElementalComp->ApplyHeat(curElementalComp->GetCurrentTemperature());

    // 오버랩이 유지되는 동안 연소 시간이 감소하도록 추적합니다.
    auto found = std::find(overlappingElementalComps.begin(),
        overlappingElementalComps.end(), otherElementalComp);
    if (found == overlappingElementalComps.end())
    {
        overlappingElementalComps.push_back(otherElementalComp);
    }
}

void FireActor::OnOverlapEnd(
    Smol::ColliderComponent* overlappedComponent,
    Smol::Actor* otherActor, Smol::ColliderComponent* otherComp){
    LOG_INFO("Overlap End.");

    ElementalComponent* otherElementalComp = otherActor->GetComponent<ElementalComponent>();
    if (nullptr == otherElementalComp)
    {
        return;
    }

    // 대상이 오버랩에서 벗어나면 연소 시간 감소를 중단합니다.
    auto found = std::find(overlappingElementalComps.begin(),
        overlappingElementalComps.end(),otherElementalComp);

    if (found != overlappingElementalComps.end())
    {
        overlappingElementalComps.erase(found);
    }
}
