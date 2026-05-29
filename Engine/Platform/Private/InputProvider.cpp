#include "EnumUtil.hpp"
#include "Primitives.hpp"
#include "InputProvider.hpp"

namespace Smol
{
    bool InputProvider::IsKeyDown(KeyCode keyCode) const noexcept{
        auto ordKey = static_cast<usize>(keyCode);
        return currentKeys.test(ordKey);
    }

    KeyState InputProvider::GetKeyState(KeyCode keyCode) const noexcept{
        auto ordKey = static_cast<usize>(keyCode);

        auto currentKeyState  = currentKeys.test(ordKey) ?
            KeyState::Pressed  : KeyState::None;
        auto previousKeyState = previousKeys.test(ordKey) ?
            KeyState::Released : KeyState::None;

        return combine(currentKeyState, previousKeyState);
    }

    void InputProvider::Reset() noexcept{
        currentKeys.reset();
        previousKeys.reset();
    }
}
