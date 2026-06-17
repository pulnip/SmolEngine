#pragma once

#include "Actor.hpp"
#include "Primitives.hpp"

class GameManager: public Smol::Actor{
    SMOL_ACTOR_BODY(GameManager)

public:
    GameManager();

    void OnStart() override;

    void OnUpdate(float) override;

    void IncreaseScore(Smol::i32 amount);

    void IncreaseArrowCount(Smol::i32 amount);

    bool GetIsPlaying(){
        return bIsPlaying;
    }

private:
    void Playing();

private:
    bool bIsPlaying = true;

    Smol::f32 remainRoundTime = 3.f;

    Smol::i32 score = 0;

    Smol::i32 arrowCount = 0;
};
