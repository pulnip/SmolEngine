smol_declare_module(Render)

target_link_libraries(SmolRender
PUBLIC
    Smol::Core
    Smol::RHI
PRIVATE
)

smol_declare_private_interface(Render)
