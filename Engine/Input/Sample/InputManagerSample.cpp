#include <iostream>
#include <SDL3/SDL_timer.h>
#include "InputManager.hpp"
#include "Window.hpp"
#include "RuntimeConfig.hpp"

using namespace Smol;

static Window& GetWindow(){
    static Window singleton(
        WindowConfig{
            .title = "InputManagerSample",
            .width = 400,
            .height = 400
        }
    );

    return singleton;
}
static InputManager& GetInputManager(){
    using enum KeyCode;

    static InputManager singleton(
        InputConfig{
            .mappings = {
                {
                    "Move",
                    ActionInfo{
                        .mappings = {W, A, S, D},
                        .count = 0
                    }
                },
                {
                    "Jump",
                    ActionInfo{
                        .mappings = {Space},
                        .count = 0
                    }
                }
            }
        },
        GetWindow().GetInputProvider()
    );

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

    void OnMoveStarted(){
        // immediate flush for debugging
        std::cout << "Move Started!" << std::endl;
    }

    void OnMoveTriggered(){
        std::cout << "Move Triggered!" << std::endl;
    }

    void OnMoveFinished(){
        std::cout << "Move Finished!" << std::endl;
    }

    void OnJumpStarted(){
        std::cout << "Jump Started!" << std::endl;
    }

    void OnJumpTriggered(){
        std::cout << "Jump Triggered!" << std::endl;
    }

    void OnJumpFinished(){
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
