#pragma once

#include "Primitives.hpp"
#include "RHIFWD.hpp"
#include "Widget.hpp"

namespace Smol
{
    struct UIContext;

    class WidgetRenderer{
        class Impl;
        RAII<Impl> impl;

    public:
        WidgetRenderer(void* sdlWindow, RHIDevice&);
        ~WidgetRenderer();

        void Draw(
            StrView uiName,
            Widget& ui,
            UIContext&,
            RHICommandList&,
            RHISwapchain*
        );
    };
}
