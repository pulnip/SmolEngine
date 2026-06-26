smol_declare_module(Render)

target_link_libraries(SmolRender
PUBLIC
    Smol::Canvas2D
    Smol::Core
    Smol::Resource
    Smol::RHI
    Smol::UI
PRIVATE
    imgui
)

smol_rhi_macro(SmolRender)
smol_declare_private_interface(Render)

if(SMOL_ENABLE_TEST)
    add_subdirectory(Sample)
endif()
