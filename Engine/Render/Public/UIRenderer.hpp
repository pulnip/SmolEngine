#pragma once

#include "ShapeRenderer.hpp"
#include "Widget.hpp"

namespace Smol
{
    namespace detail{
        class UIRendererInitializer final{
        public:
            UIRendererInitializer();
        };
    }

    class UIRenderer final{
    private:
        detail::UIRendererInitializer initializer;
        ShapeRenderer shapeRenderer;
        Widget debugWidget;

    public:
        UIRenderer(void* sdlWindow, RHIDevice&, Widget&& debugWidget);
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
