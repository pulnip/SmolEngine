smol_declare_module(Reflection)

target_link_libraries(SmolReflection
PUBLIC
    Smol::Core
    Smol::Serialization
)

smol_declare_private_interface(Reflection)

# if(SMOL_ENABLE_TEST)
#     smol_declare_test(Reflection
#         DIRECTORY Test
#         DEPENDS Smol::Reflection::Private
#         LABELS "unit"
#     )
# endif()
