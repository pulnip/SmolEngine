#include "SpawnContext.hpp"
#include "MockInputManager.hpp"

namespace Smol
{
    IInputManager& SpawnContext::mockInputManager(){
        static MockInputManager inputManager;

        return inputManager;
    }
}
