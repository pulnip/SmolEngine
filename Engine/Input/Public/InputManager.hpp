#pragma once

#include <tuple>
#include <unordered_map>
#include <vector>
#include "InputAction.hpp"
#include "InputConfig.hpp"
#include "KeyCode.hpp"
#include "Semantics.hpp"
#include "StringUtil.hpp"
#include "SlotMap.hpp"

namespace Smol
{
    enum TriggerEvent{
        Started = 0,
        Triggered = 1,
        Completed = 2
    };

    class InputProvider;

    class InputManager final{
    public:
        using Callback = InputAction::Callback;
        using Handle = InputAction::Handle;

    private:
        // Dependency Injection from OS
        InputProvider* provider = nullptr;

        StringHashMap<KeyCode> mappings;

        SlotMap<Callback> callbacks;
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
        auto BindAction(StrView action, TriggerEvent event,
            T* actor, void(T::*func)()
        ){
            return bindAction(action, event, [actor, func](){
                (actor->*func)();
            });
        }
        void UnbindAction(Handle);

    private:
        [[nodiscard]]
        InputAction bindAction(StrView action, TriggerEvent, Callback&&);
    };
}
