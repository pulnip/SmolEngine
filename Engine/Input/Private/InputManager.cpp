#include <span>
#include <utility>
#include "Assert.hpp"
#include "HashUtil.hpp"
#include "InputManager.hpp"
#include "InputProvider.hpp"
#include "LinearAlgebra.hpp"
#include "VariantUtil.hpp"

namespace{
    auto Apply(std::span<const Smol::InputModifier> modifiers, Smol::Vec3 initValue){
        using namespace Smol;

        auto value = InputValue(initValue);

        for(auto& modifier: modifiers)
            value = Apply(modifier, value);

        return value;
    }
}

namespace Smol
{
    usize InputManager::ActionKeyHash::operator()(const ActionKey& key) const{
        return hashAll(key.actionName, key.event);
    }

    InputManager::InputManager(
        InputConfig&& config,
        const InputProvider* provider
    )
        : provider(provider)
        , mappings(std::move(config.mappings))
    {}

    void InputManager::NewFrame(){
        handleActionStarted();
        handleActionTriggered();
        // Notice! key could be Pressed and Released on Same frame.
        handleActionFinished();
    }

    void InputManager::handleActionStarted(){
        for(auto& action: mappings){
            const auto isReadyToStart = action.count == 0;
            bool anyKeyPressed = false;
            auto value = InputValue(zeros());

            for(const auto& binding: action.mappings){
                std::visit(overload{
                    [&](const KeyCode& keyCode){
                        if(provider->IsKeyPressed(keyCode)){
                            anyKeyPressed = true;
                            ++action.count;

                            // Additive Blending
                            // (1, 0, 0) for keyboard
                            value += ::Apply(binding.modifiers, unitX());
                        }
                    },
                    [&](const MouseButton& button){
                        if(provider->IsKeyPressed(button)){
                            anyKeyPressed = true;
                            ++action.count;

                            // dpos for mouse
                            Vec3 dpos = provider->GetMouseDPos();
                            value += ::Apply(binding.modifiers, dpos);
                        }
                    }
                }, binding.cond);
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
                std::visit(overload{
                    [&](const KeyCode& keyCode){
                        if(provider->IsKeyDown(keyCode)){
                            anyKeyHeld = true;

                            value += ::Apply(binding.modifiers, unitX());
                        }
                    },
                    [&](const MouseButton& button){
                        if(provider->IsKeyDown(button)){
                            anyKeyHeld = true;

                            value += ::Apply(binding.modifiers, provider->GetMouseDPos());
                        }
                    }
                }, binding.cond);
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
                std::visit(overload{
                    [&](const KeyCode& keyCode){
                        if(provider->IsKeyReleased(keyCode)){
                            anyKeyReleased = true;
                            --action.count;

                            value += ::Apply(binding.modifiers, unitX());
                        }
                    },
                    [&](const MouseButton& button){
                        if(provider->IsKeyReleased(button)){
                            anyKeyReleased = true;
                            --action.count;

                            Vec3 dpos = provider->GetMouseDPos();
                            value += ::Apply(binding.modifiers, dpos);
                        }
                    }
                }, binding.cond);

            }

            const auto isActionFinished = action.count == 0 && anyKeyReleased;
            if(isActionFinished)
                fireAction(ActionKey{
                    .actionName = action.name,
                    .event = TriggerEvent::Finished
                }, value);
        }
    }

    Vec2 InputManager::GetMousePos() const noexcept{
        return provider->GetMousePos();
    }

    void InputManager::UnbindAction(Handle handle){
        callbacks.Remove(handle);

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
        auto handle = callbacks.Emplace(std::move(callback));

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
            callbacks.GetRef(handle)(value);
    }
}
