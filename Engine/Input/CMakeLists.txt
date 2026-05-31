smol_declare_module(Input)

target_link_libraries(SmolInput
PUBLIC
    Smol::Core
    Smol::Platform
    Smol::Serialization
PRIVATE
    Smol::Log
)

if(SMOL_ENABLE_TEST)
    smol_declare_test(Input
        DIRECTORY Test
        DEPENDS
            Smol::Input
            Smol::Serialization
        LABELS "unit"
    )
    add_subdirectory(Sample)
endif()
