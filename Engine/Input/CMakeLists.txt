smol_declare_module(Input)

target_link_libraries(SmolInput
PUBLIC
    Smol::Core
    Smol::Serialization
PRIVATE
    Smol::Log
)

smol_declare_private_interface(Input)

if(SMOL_ENABLE_TEST)
    smol_declare_test(Input
        DIRECTORY Test
        DEPENDS
            Smol::Input::Private
            Smol::Serialization
        LABELS "unit"
    )
endif()
