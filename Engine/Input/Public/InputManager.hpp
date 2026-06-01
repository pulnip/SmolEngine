#pragma once

#include <unordered_map>
#include <vector>
#include "InputAction.hpp"
#include "InputConfig.hpp"
#include "Semantics.hpp"
#include "SlotMap.hpp"

namespace Smol
{
    class InputProvider;

    // Input Manager for Single Context
    class InputManager final{
    public:
        using Callback = InputAction::Callback;
        using Handle = InputAction::Handle;

    private:
        // Dependency Injection from OS
        const InputProvider* provider = nullptr;

        ActionMappings mappings;

        SlotMap<Callback> callbacks;
        // Action Name with Trigger Event
        struct ActionKey{
            ActionName actionName;
            TriggerEvent event;

            friend bool operator==(const ActionKey& lhs, const ActionKey& rhs){
                return lhs.actionName == rhs.actionName && lhs.event == rhs.event;
            }
        };
        struct ActionKeyHash{
            usize operator()(const ActionKey& key) const;
        };
        std::unordered_map<ActionKey, std::vector<Handle>, ActionKeyHash> actionMap;
        // for easy remove
        std::unordered_map<Handle, ActionKey> handleToAction;

    public:
        InputManager() = default;
        ~InputManager() = default;
        SMOL_DECLARE_MOVE_ONLY(InputManager)

        InputManager(
            const InputConfig&,
            const InputProvider*
        );

        void NewFrame();

        // Notice! Not Guarantee the order in Single frame
        [[nodiscard]]
        InputAction BindAction(StrView action, TriggerEvent, Callback&&);
        void UnbindAction(Handle);

    private:
        void handleActionStarted();
        void handleActionTriggered();
        void handleActionFinished();

        void fireAction(const ActionKey&, InputValue);
    };
}
