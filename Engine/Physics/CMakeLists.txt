smol_declare_module(Physics)

target_link_libraries(SmolPhysics
PUBLIC
    Smol::Core
PRIVATE
    Smol::Reflection
)
