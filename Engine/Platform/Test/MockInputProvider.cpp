#include "MockInputProvider.hpp"

namespace Smol
{
    void MockInputProvider::Poll(){
        previousKeys = currentKeys;
        currentKeys = transitionKeys;
    }

    void MockInputProvider::Reset(){
        InputProvider::Reset();

        transitionKeys.reset();
    }

    void MockInputProvider::PressKey(KeyCode keyCode){
        auto ordKey = static_cast<size_t>(keyCode);
        transitionKeys.set(ordKey);
    }

    void MockInputProvider::ReleaseKey(KeyCode keyCode){
        auto ordKey = static_cast<size_t>(keyCode);
        transitionKeys.reset(ordKey);
    }
}
