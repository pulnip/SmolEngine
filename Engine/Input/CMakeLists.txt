smol_declare_module(Input)

target_link_libraries(SmolInput
PUBLIC
    Smol::Core
)

# if(SMOL_ENABLE_TEST)
#     smol_declare_test(Input
#         DIRECTORY Test
#         DEPENDS Smol::Input::Private
#         LABELS "unit"
#     )
# endif()
