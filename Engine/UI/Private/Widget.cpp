#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include "Widget.hpp"

namespace Smol
{
    void Checkbox::submit(UIContext& ctx){
        if(ImGui::Checkbox(label.c_str(), &v))
            onChanged(ctx, v);
    }
}
