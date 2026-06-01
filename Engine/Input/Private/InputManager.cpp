#include <span>
#include <utility>
#include "Assert.hpp"
#include "HashUtil.hpp"
#include "InputManager.hpp"
#include "InputProvider.hpp"

namespace Smol
{
    usize InputManager::ActionKeyHash::operator()(const ActionKey& key) const{
        return hashAll(key.actionName, key.event);
    }

    InputManager::InputManager(
        const InputConfig& config,
        const InputProvider* provider
    )
        : provider(std::move(provider))
        , mappings(config.mappings)
    {}

    void InputManager::NewFrame(){
        handleActionStarted();
        handleActionTriggered();
        // Notice! key could be Pressed and Released on Same frame.
        handleActionFinished();
    }

    inline InputValue Apply(std::span<const InputModifier> modifiers){
        auto value = InputValue(unitX());

        for(auto& modifier: modifiers)
            value = Apply(modifier, value);

        return value;
    }

    void InputManager::handleActionStarted(){
        for(auto& action: mappings){
            const auto isReadyToStart = action.count == 0;
            bool anyKeyPressed = false;
            auto value = InputValue(zeros());

            for(const auto& binding: action.mappings){
                if(provider->IsKeyPressed(binding.keyCode)){
                    anyKeyPressed = true;
                    ++action.count;

                    // Additive Blending
                    value += Apply(binding.modifiers);
                }
            }

            const auto isActionStarted = isReadyToStart && anyKeyPressed;
            if(isActionStarted) [[unlikely]]
                fireAction(ActionKey{
                    .actionName = action.name,
                    .event = TriggerEvent::Started
                }, value);
        }
    }

    void InputManager::handleActionTriggered(){
        for(const auto& action: mappings){
            bool anyKeyHeld = false;
            auto value = InputValue(zeros());

            for(const auto& binding: action.mappings){
                if(provider->IsKeyDown(binding.keyCode)){
                    anyKeyHeld = true;

                    value += Apply(binding.modifiers);
                }
            }

            if(anyKeyHeld)
                fireAction(ActionKey{
                    .actionName = action.name,
                    .event = TriggerEvent::Triggered
                }, value);
        }
    }

    void InputManager::handleActionFinished(){
        for(auto& action: mappings){
            bool anyKeyReleased = false;
            auto value = InputValue(zeros());

            for(const auto& binding: action.mappings){
                if(provider->IsKeyReleased(binding.keyCode)){
                    anyKeyReleased = true;
                    --action.count;

                    value += Apply(binding.modifiers);
                }
            }

            const auto isActionFinished = action.count == 0 && anyKeyReleased;
            if(isActionFinished)
                fireAction(ActionKey{
                    .actionName = action.name,
                    .event = TriggerEvent::Finished
                }, value);
        }
    }

    void InputManager::UnbindAction(Handle handle){
        callbacks.remove(handle);

        auto it = handleToAction.find(handle);
        SMOL_ASSERT(it != handleToAction.end());

        const ActionKey& actionKey = it->second;
        std::vector<Handle>& v = actionMap[actionKey];
        std::erase(v, handle);
        handleToAction.erase(it);
    }

    InputAction InputManager::BindAction(
        StrView str,
        TriggerEvent event,
        Callback&& callback
    ){
        auto handle = callbacks.emplace(std::move(callback));

        ActionKey key{
            .actionName = Str(str),
            .event = event
        };
        actionMap[key].push_back(handle);
        handleToAction[handle] = std::move(key);

        return InputAction(handle, this);
    }

    void InputManager::fireAction(const ActionKey& key, InputValue value){
        auto it = actionMap.find(key);
        // Binded funtion not exist
        if(it == actionMap.end())
            return;

        const auto& handles = it->second;
        // fire action
        for(const auto& handle: handles)
            callbacks.get(handle)(value);
    }
}
