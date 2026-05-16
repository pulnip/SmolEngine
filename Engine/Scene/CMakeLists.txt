smol_declare_module(Scene)

target_link_libraries(SmolScene
PUBLIC
    Smol::Core
PRIVATE
    Smol::Reflection
)

smol_declare_private_interface(Scene)
