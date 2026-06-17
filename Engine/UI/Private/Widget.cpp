#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include "Widget.hpp"
#include "Primitives.hpp"

namespace{
    enum class Axis{
        horizontal,
        vertical
    };

    struct Flex{
        Axis axis;
        std::vector<Smol::Widget> children;
        double spacing = 0.0;

        void submit(Smol::UIContext& ctx){
            ImGui::BeginGroup();
            for(Smol::usize i = 0; i < children.size(); ++i){
                if(i > 0 && axis == Axis::horizontal)
                    ImGui::SameLine(0, spacing);

                std::visit([&ctx](auto& widget){
                    widget.submit(ctx);
                }, children[i]);
            }
            ImGui::EndGroup();
        }
    };
}

namespace Smol
{
    void Checkbox::submit(UIContext& ctx){
        if(ImGui::Checkbox(label.c_str(), &v))
            onChanged(ctx, v);
    }

    Widget Row(
        std::vector<Widget>&& children,
        double spacing
    ){
        return Box(Flex{
            .axis = Axis::horizontal,
            .children = std::move(children)
        });
    }

    Widget Column(
        std::vector<Widget>&& children,
        double spacing
    ){
        return Box(Flex{
            .axis = Axis::vertical,
            .children = std::move(children)
        });
    }
}
