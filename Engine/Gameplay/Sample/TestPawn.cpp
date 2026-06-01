#include <iostream>
#include "CharacterController.hpp"
#include "InputComponent.hpp"
#include "TestPawn.hpp"

namespace Smol
{
    SMOL_ACTOR(TestPawn)
    SMOL_ACTOR_END(TestPawn)

    void TestPawn::PossessedBy(CharacterController& controller){
        auto& input = controller.GetInputComponent();

        input.BindAction(
            "Move", TriggerEvent::Started,
            this, &TestPawn::OnMoveStarted
        );
        input.BindAction(
            "Move", TriggerEvent::Triggered,
            this, &TestPawn::OnMoveTriggered
        );
        input.BindAction(
            "Move", TriggerEvent::Finished,
            this, &TestPawn::OnMoveFinished
        );

        input.BindAction(
            "Jump", TriggerEvent::Started,
            this, &TestPawn::OnJumpStarted
        );
        input.BindAction(
            "Jump", TriggerEvent::Triggered,
            this, &TestPawn::OnJumpTriggered
        );
        input.BindAction(
            "Jump", TriggerEvent::Finished,
            this, &TestPawn::OnJumpFinished
        );
    }

    void TestPawn::OnMoveStarted(InputValue v){
        auto dir = v.GetAxis3D();

        // immediate flush for debugging
        std::cout << "Start Move to " <<
            dir.x << ", " << dir.y << ", " << dir.z
        << std::endl;
    }

    void TestPawn::OnMoveTriggered(InputValue v){
        auto dir = v.GetAxis3D();

        std::cout << "Trigger Move to " <<
            dir.x << ", " << dir.y << ", " << dir.z
        << std::endl;
    }

    void TestPawn::OnMoveFinished(InputValue v){
        auto dir = v.GetAxis3D();

        std::cout << "Finish Move to " <<
            dir.x << ", " << dir.y << ", " << dir.z
        << std::endl;
    }

    void TestPawn::OnJumpStarted(InputValue v){
        std::cout << "Jump Started!" << std::endl;
    }

    void TestPawn::OnJumpTriggered(InputValue v){
        std::cout << "Jump Triggered!" << std::endl;
    }

    void TestPawn::OnJumpFinished(InputValue v){
        std::cout << "Jump Finished!" << std::endl;
    }
}
