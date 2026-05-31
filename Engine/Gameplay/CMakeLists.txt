smol_declare_module(Gameplay)

target_link_libraries(SmolGameplay
PUBLIC
    Smol::Core
    Smol::Reflection
PRIVATE
    Smol::Input
)
