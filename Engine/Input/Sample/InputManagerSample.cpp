#include <iostream>
#include <SDL3/SDL_timer.h>
#include "InputConfig.hpp"
#include "InputManager.hpp"
#include "InputModifier.hpp"
#include "Window.hpp"
#include "RuntimeConfig.hpp"

using namespace Smol;

static Window& GetWindow(){
    static Window singleton(WindowConfig{
        .title = "InputManagerSample",
        .width = 400,
        .height = 400
    });

    return singleton;
}
static InputManager& GetInputManager(){
    using enum KeyCode;
    using enum SwizzleOrder;

    static InputManager singleton(InputConfig{
        .mappings = {
            ActionInfo{
                .name = "Move",
                .mappings = {
                    KeyBinding{
                        .keyCode = W,
                        .modifiers = {
                            SwizzleModifier(ZYX)
                        }
                    },
                    KeyBinding{
                        .keyCode = A,
                        .modifiers = {
                            NegateModifier(
                                true,
                                false,
                                false
                            )
                        }
                    },
                    KeyBinding{
                        .keyCode = S,
                        .modifiers = {
                            SwizzleModifier(ZYX),
                            NegateModifier(
                                false,
                                false,
                                true
                            ),
                        }
                    },
                    KeyBinding{
                        .keyCode = D
                    }
                },
                .count = 0
            },
            ActionInfo{
                .name = "Jump",
                .mappings = {
                    KeyBinding{
                        .keyCode = Space
                    },
                },
                .count = 0
            }
        }
    }, GetWindow().GetInputProvider());

    return singleton;
}

struct TestActor{
private:
    InputAction moveActionStarted;
    InputAction moveActionTriggered;
    InputAction moveActionFinished;

    InputAction jumpActionStarted;
    InputAction jumpActionTriggered;
    InputAction jumpActionFinished;

public:
    TestActor(){
        moveActionStarted = GetInputManager().BindAction(
            "Move", TriggerEvent::Started,
            this, &TestActor::OnMoveStarted
        );
        moveActionTriggered = GetInputManager().BindAction(
            "Move", TriggerEvent::Triggered,
            this, &TestActor::OnMoveTriggered
        );
        moveActionFinished = GetInputManager().BindAction(
            "Move", TriggerEvent::Finished,
            this, &TestActor::OnMoveFinished
        );

        jumpActionStarted = GetInputManager().BindAction(
            "Jump", TriggerEvent::Started,
            this, &TestActor::OnJumpStarted
        );
        jumpActionTriggered = GetInputManager().BindAction(
            "Jump", TriggerEvent::Triggered,
            this, &TestActor::OnJumpTriggered
        );
        jumpActionFinished = GetInputManager().BindAction(
            "Jump", TriggerEvent::Finished,
            this, &TestActor::OnJumpFinished
        );
    }
    ~TestActor() = default;

    void OnMoveStarted(InputValue v){
        auto dir = v.GetAxis3D();

        // immediate flush for debugging
        std::cout << "Start Move to " <<
            dir.x << ", " << dir.y << ", " << dir.z
        << std::endl;
    }

    void OnMoveTriggered(InputValue v){
        auto dir = v.GetAxis3D();

        std::cout << "Trigger Move to " <<
            dir.x << ", " << dir.y << ", " << dir.z
        << std::endl;
    }

    void OnMoveFinished(InputValue v){
        auto dir = v.GetAxis3D();

        std::cout << "Finish Move to " <<
            dir.x << ", " << dir.y << ", " << dir.z
        << std::endl;
    }

    void OnJumpStarted(InputValue v){
        std::cout << "Jump Started!" << std::endl;
    }

    void OnJumpTriggered(InputValue v){
        std::cout << "Jump Triggered!" << std::endl;
    }

    void OnJumpFinished(InputValue v){
        std::cout << "Jump Finished!" << std::endl;
    }
};

int main(int, char*[]){
    auto& window = GetWindow();
    auto& manager = GetInputManager();
    TestActor actor;

    while(true){
        if(!window.ProcessEvents()) [[unlikely]]
            break;

        manager.NewFrame();
        // Without Delay, Loop too tight!
        SDL_Delay(5);
    }
}
