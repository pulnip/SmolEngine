#pragma once

#include <functional>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "InputConfig.hpp"
#include "InputProvider.hpp"
#include "KeyCode.hpp"
#include "Semantics.hpp"
#include "StringUtil.hpp"
#include "SlotMap.hpp"

namespace Smol
{
    using ActionCallback = std::function<void()>;

    enum TriggerEvent{
        Started = 0,
        Triggered = 1,
        Completed = 2
    };

    class InputManager final{
    public:
        using Handle = GenericHandle<ActionCallback>;

    private:
        // Dependency Injection from OS
        InputProvider* provider = nullptr;

        StringHashMap<KeyCode> mappings;

        SlotMap<ActionCallback> callbacks;
        StringHashMap<std::vector<std::tuple<TriggerEvent, Handle>>> actionMap;
        // for easy remove
        std::unordered_map<Handle, Str> handleToAction;

    public:
        InputManager() = default;
        ~InputManager() = default;
        SMOL_DECLARE_MOVE_ONLY(InputManager)

        InputManager(
            const InputConfig&,
            InputProvider*
        );

        void NewFrame();

        // Notice! Not Guarantee the order in Single frame
        template<typename T>
        [[nodiscard]]
        Handle BindAction(StrView action, TriggerEvent event, T* actor, void(T::*func)()){
            return bindAction(action, event, [actor, func](){
                (actor->*func)();
            });
        }
        void UnbindAction(Handle);

    private:
        [[nodiscard]]
        Handle bindAction(StrView action, TriggerEvent, ActionCallback&&);
    };
}
