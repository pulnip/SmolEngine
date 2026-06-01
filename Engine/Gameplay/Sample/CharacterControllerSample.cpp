#include <SDL3/SDL_timer.h>
#include "ActorFactory.hpp"
#include "CharacterController.hpp"
#include "InputConfig.hpp"
#include "InputManager.hpp"
#include "InputModifier.hpp"
#include "Pawn.hpp"
#include "RuntimeConfig.hpp"
#include "Window.hpp"

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

int main(int, char*[]){
    auto& window = GetWindow();
    auto& manager = GetInputManager();

    ServiceLocator locator{
        .inputManager = manager
    };
    auto controller = CreateActor("CharacterController");
    controller->Init(locator);
    auto pawn = CreateActor("TestPawn");
    pawn->Init(locator);

    auto pawnPtr = static_cast<Pawn*>(pawn.get());
    auto controllerPtr = static_cast<CharacterController*>(controller.get());
    pawnPtr->PossessedBy(*controllerPtr);

    while(true){
        if(!window.ProcessEvents()) [[unlikely]]
            break;

        manager.NewFrame();
        // Without Delay, Loop too tight!
        SDL_Delay(5);
    }
}
