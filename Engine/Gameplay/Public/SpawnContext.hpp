#pragma once

#include <filesystem>
#include "DOM.hpp"
#include "Resource.hpp"

namespace Smol
{
    template<Resource T>
    class ResourceManager;
    class IInputManager;
    class RHIDevice;
    class SpriteRenderer;
    class World;
    class Actor;

    struct SpawnContext{
        static IInputManager& mockInputManager();

        const DOM::Value& dom = {};
        const std::filesystem::path contentRoot = "Content/";

        ResourceManager<SpriteResource>* spriteManager = nullptr;
        IInputManager& inputManager = mockInputManager();
        // TODO. mock device?
        RHIDevice* device = nullptr;
        // TODO. mock renderer?
        SpriteRenderer* spriteRenderer = nullptr;
        // TODO. mock world?
        World* world = nullptr;

        Actor* owner = nullptr;

        auto WithDOM(const DOM::Value& node) const noexcept{
            return SpawnContext{
                .dom = node,
                .contentRoot = contentRoot,
                .spriteManager = spriteManager,
                .inputManager = inputManager,
                .device = device,
                .spriteRenderer = spriteRenderer,
                .world = world,
                .owner = owner
            };
        }
        auto WithOwner(Actor* actor) const noexcept{
            return SpawnContext{
                .dom = dom,
                .contentRoot = contentRoot,
                .spriteManager = spriteManager,
                .inputManager = inputManager,
                .device = device,
                .spriteRenderer = spriteRenderer,
                .world = world,
                .owner = actor
            };
        }
    };
}
