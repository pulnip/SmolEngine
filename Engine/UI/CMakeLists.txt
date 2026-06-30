smol_declare_module(UI)

target_link_libraries(SmolUI
PUBLIC
    Smol::CommonRHI
    Smol::Core
    imgui
PRIVATE
    Smol::RHI
)

if(WIN32)
    target_link_libraries(SmolUI
    PRIVATE
        Smol::DX11RHI
    )
elseif(APPLE)
    target_link_libraries(SmolUI
    PRIVATE
        Smol::MetalRHI
    )
endif()