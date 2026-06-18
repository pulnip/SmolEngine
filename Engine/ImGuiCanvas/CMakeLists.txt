smol_declare_module(ImGuiCanvas)

target_link_libraries(SmolImGuiCanvas
PUBLIC
    Smol::Canvas2D
PRIVATE
    imgui
    Smol::Log
)
