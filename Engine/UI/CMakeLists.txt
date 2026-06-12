smol_declare_module(UI)

target_link_libraries(SmolUI
PUBLIC
    Smol::CommonRHI
    Smol::Core
    imgui
PRIVATE
    Smol::RHI
)

smol_rhi_macro(UI)
if(RENDER_BACKEND STREQUAL "DX11")
    target_link_libraries(SmolUI
    PRIVATE
        Smol::DX11RHI
    )
elseif(RENDER_BACKEND STREQUAL "Metal")
    target_link_libraries(SmolUI
    PRIVATE
        Smol::MetalRHI
    )
endif()