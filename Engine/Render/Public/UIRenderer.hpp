#pragma once

#include <filesystem>
#include "ShapeRenderer.hpp"
#include "Widget.hpp"

namespace Smol
{
    namespace detail{
        class UIRendererInitializer final{
        public:
            UIRendererInitializer(const std::filesystem::path& contentRoot);
        };
    }

    class UIRenderer final{
    private:
        detail::UIRendererInitializer initializer;
        ShapeRenderer shapeRenderer;
        Widget debugWidget;

    public:
        UIRenderer(void* sdlWindow, RHIDevice&,
            const std::filesystem::path& contentRoot,
            Widget&& debugWidget
        );
        ~UIRenderer();

        void Draw(
            RHICommandList&,
            RHISwapchain*
        );

        ShapeRenderer& GetShapeRenderer(){
            return shapeRenderer;
        }
    };
}
