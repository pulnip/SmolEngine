#include "Assert.hpp"
#include <utility>
#include "InputManager.hpp"
#include "InputProvider.hpp"

namespace Smol
{
    InputManager::InputManager(
        const InputConfig& config,
        const InputProvider* provider
    )
        : provider(std::move(provider))
        , mappings(config.mappings)
    {}

    void InputManager::NewFrame(){
        for(const auto& [name, actions]: actionMap){
            auto it = mappings.find(name);
            if(it == mappings.end())
                continue;

            for(const auto& [event, handle]: actions){
                using enum TriggerEvent;

                switch(event){
                case Started:
                    if(provider->IsKeyPressed(it->second))
                        callbacks.get(handle)();
                    break;
                case Triggered:
                    if(provider->IsKeyDown(it->second))
                        callbacks.get(handle)();
                    break;
                case Completed:
                    if(provider->IsKeyReleased(it->second))
                        callbacks.get(handle)();
                    break;
                default:
                    std::unreachable();
                }
            }
        }
    }

    InputAction InputManager::bindAction(
        StrView str,
        TriggerEvent event,
        Callback&& callback
    ){
        auto handle = callbacks.emplace(std::move(callback));

        auto action = std::make_tuple(event, handle);
        actionMap[Str(str)].push_back(std::move(action));
        handleToAction[handle] = str;

        return InputAction(handle, this);
    }

    void InputManager::UnbindAction(Handle handle){
        callbacks.remove(handle);

        auto it = handleToAction.find(handle);
        SMOL_ASSERT(it != handleToAction.end());

        // std::erase(actionMap[it->second], handle);
        handleToAction.erase(it);
    }
}
