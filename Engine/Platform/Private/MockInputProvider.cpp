#include "MockInputProvider.hpp"
#include "Primitives.hpp"

namespace Smol
{
    void MockInputProvider::Poll(){
        previousKeys = currentKeys;
        currentKeys = transitionKeys;

        transitionKeys.reset();
    }

    void MockInputProvider::Reset(){
        InputProvider::Reset();

        transitionKeys.reset();
    }

    void MockInputProvider::PressKey(KeyCode keyCode){
        auto ordKey = static_cast<usize>(keyCode);
        transitionKeys.set(ordKey);
    }

    void MockInputProvider::ReleaseKey(KeyCode keyCode){
        auto ordKey = static_cast<usize>(keyCode);
        transitionKeys.reset(ordKey);
    }
}
