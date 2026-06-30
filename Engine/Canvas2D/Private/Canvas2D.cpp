#include <utility>
#include "Canvas2D.hpp"

namespace Smol
{
    // each platform should implement this function
    Canvas2DRAII CreateD2DCanvas(RHIDevice&);
    Canvas2DRAII CreateImGuiCanvas();

    Canvas2DRAII CreateCanvas(RHIDevice& device, CanvasBackend backend){
        using enum CanvasBackend;

        switch(backend){
    #if defined(_WIN32)
        case Direct2D: return CreateD2DCanvas(device);
    #endif
        case ImGui:    return CreateImGuiCanvas();
        default:
            std::unreachable();
        }
    }
}
