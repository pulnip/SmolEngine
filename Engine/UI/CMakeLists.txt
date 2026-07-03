smol_declare_module(UI)

target_link_options(SmolUI
PRIVATE
    ${RENDER_BACKEND_LINK_OPTIONS}
)
target_link_libraries(SmolUI
PUBLIC
    Smol::CommonRHI
    Smol::Core
    imgui
PRIVATE
    ${RENDER_BACKENDS}
)
