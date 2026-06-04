#include <SDL3/SDL_timer.h>
#include "ActorRegistry.hpp"
#include "Assert.hpp"
#include "CharacterController.hpp"
#include "InputConfig.hpp"
#include "InputManager.hpp"
#include "InputModifier.hpp"
#include "Pawn.hpp"
#include "RuntimeConfig.hpp"
#include "SDLInputProvider.hpp"
#include "SDLWindow.hpp"
#include "SpawnContext.hpp"

using namespace Smol;

int main(int, char*[]){
    SDLWindow window(WindowConfig{
        .title = "InputManagerSample",
        .width = 400,
        .height = 400
    });

    SDLInputProvider inputProvider;
    InputManager inputManager(InputConfig{
        .mappings = {
            ActionInfo{
                .name = "Move",
                .mappings = {
                    KeyBinding{
                        .keyCode = KeyCode::W,
                        .modifiers = {
                            SwizzleModifier(ZYX)
                        }
                    },
                    KeyBinding{
                        .keyCode = KeyCode::A,
                        .modifiers = {
                            NegateModifier(
                                true,
                                false,
                                false
                            )
                        }
                    },
                    KeyBinding{
                        .keyCode = KeyCode::S,
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
                        .keyCode = KeyCode::D
                    }
                },
                .count = 0
            },
            ActionInfo{
                .name = "Jump",
                .mappings = {
                    KeyBinding{
                        .keyCode = KeyCode::Space
                    },
                },
                .count = 0
            }
        }
    }, &inputProvider);

    SpawnContext spawnContext{
        .inputManager = inputManager
    };
    auto controller = CreateActor("CharacterController", spawnContext);
    SMOL_ASSERT(controller != nullptr);
    auto pawn = CreateActor("TestPawn", spawnContext);
    SMOL_ASSERT(pawn != nullptr);

    auto controllerPtr = static_cast<CharacterController*>(controller.get());
    auto pawnPtr = static_cast<Pawn*>(pawn.get());
    pawnPtr->PossessedBy(*controllerPtr);

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

        inputManager.NewFrame();
        // Without Delay, Loop too tight!
        SDL_Delay(5);
    }
}
