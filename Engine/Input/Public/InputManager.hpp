#pragma once

#include <functional>
#include <vector>
#include "CoreFWD.hpp"
#include "InputProvider.hpp"
#include "Semantics.hpp"
#include "StringUtil.hpp"

namespace Smol
{
    using ActionCallback = std::function<void()>;

    class InputManager final{
    private:
        // Dependency Injection from OS
        RAII<InputProvider> provider = nullptr;

        StringHashMap<std::vector<ActionCallback>> actionMap;

    public:
        InputManager(RAII<InputProvider>);
        ~InputManager() = default;
        SMOL_DECLARE_PINNED(InputManager)



        template<typename T>
        void BindAction(StrView action, T* actor, void(T::*func)()){
            bindAction(action, [actor, func](){
                (actor->*func)();
            });
        }

    private:
        void bindAction(StrView action, ActionCallback);
    };
}
