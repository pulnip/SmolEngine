#include <iostream>
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
    static InputManager singleton(
        InputConfig{
            .mappings = {
            {"Move", KeyCode::W},
            {"Jump", KeyCode::Space}
            }
        },
        GetWindow().GetInputProvider()
    );

    return singleton;
}

struct TestActor{
private:
    InputAction moveAction;
    InputAction jumpAction;

public:
    TestActor(){
        moveAction = GetInputManager().BindAction(
            "Move", TriggerEvent::Triggered,
            this, &TestActor::OnMove
        );
        jumpAction = GetInputManager().BindAction(
            "Jump", TriggerEvent::Triggered,
            this, &TestActor::OnJump
        );
    }
    ~TestActor() = default;

    void OnMove(){
        std::cout << "Move!" << std::endl;
    }

    void OnJump(){
        std::cout << "Jump!" << std::endl;
    }
};

int main(int, char*[]){
    auto& window = GetWindow();
    auto& manager = GetInputManager();
    TestActor actor;

    while(true){
        [[unlikely]] if(!window.ProcessEvents())
            break;
        manager.NewFrame();
    }
}
