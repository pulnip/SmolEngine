#include <iostream>
#include <SDL3/SDL_timer.h>
#include "InputConfig.hpp"
#include "InputManager.hpp"
#include "InputModifier.hpp"
#include "RuntimeConfig.hpp"
#include "SDLInputProvider.hpp"
#include "SDLWindow.hpp"

using namespace Smol;

static SDLInputProvider& GetInputProvider(){
    static SDLInputProvider inputProvider;

    return inputProvider;
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
    }, &GetInputProvider());

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
            [&](InputValue v){ OnMoveStarted(v); }
        );
        moveActionTriggered = GetInputManager().BindAction(
            "Move", TriggerEvent::Triggered,
            [&](InputValue v){ OnMoveTriggered(v); }
        );
        moveActionFinished = GetInputManager().BindAction(
            "Move", TriggerEvent::Finished,
            [&](InputValue v){ OnMoveFinished(v); }
        );

        jumpActionStarted = GetInputManager().BindAction(
            "Jump", TriggerEvent::Started,
            [&](InputValue v){ OnJumpStarted(v); }
        );
        jumpActionTriggered = GetInputManager().BindAction(
            "Jump", TriggerEvent::Triggered,
            [&](InputValue v){ OnJumpTriggered(v); }
        );
        jumpActionFinished = GetInputManager().BindAction(
            "Jump", TriggerEvent::Finished,
            [&](InputValue v){ OnJumpFinished(v); }
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
    SDLWindow window(WindowConfig{
        .title = "InputManagerSample",
        .width = 400,
        .height = 400
    });

    auto& inputProvider = GetInputProvider();
    auto& manager = GetInputManager();
    TestActor actor;

    while(true){
        bool keepRunning = true;

        inputProvider.NewFrame();

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
            case SDL_EVENT_QUIT: [[unlikely]]
                keepRunning = false;
                break;
            // Keyboard Event
            case SDL_EVENT_KEY_DOWN:
                [[fallthrough]];
            case SDL_EVENT_KEY_UP:
                inputProvider.OnPlatformEvent(event.key);
                break;
            }

            if(!keepRunning) [[unlikely]]
                break;

            // Window Event
            if(SDL_EVENT_WINDOW_FIRST <= event.type && event.type <= SDL_EVENT_WINDOW_LAST){
                window.OnPlatformEvent(event.window);
            }
        }

        if(!keepRunning) [[unlikely]]
            break;

        manager.NewFrame();
        // Without Delay, Loop too tight!
        SDL_Delay(5);
    }
}
