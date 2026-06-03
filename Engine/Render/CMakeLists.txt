smol_declare_module(Render)

target_link_libraries(SmolRender
PUBLIC
    Smol::Core
    Smol::RHI
PRIVATE
)

smol_rhi_macro(Render)
smol_declare_private_interface(Render)

if(SMOL_ENABLE_TEST)
    add_subdirectory(Sample)
endif()
