smol_declare_module(Serialization)

target_compile_definitions(SmolSerialization
PRIVATE
    TOML_EXCEPTIONS=1
)

target_link_libraries(SmolSerialization
PUBLIC
    Smol::Core
PRIVATE
    stb
    tomlplusplus::tomlplusplus
    Smol::Log
)

smol_declare_private_interface(Serialization)

if(SMOL_ENABLE_TEST)
    smol_declare_test(Serialization
        DIRECTORY Test
        DEPENDS Smol::Serialization::Private
        LABELS "unit"
    )

    add_subdirectory(Sample)
endif()
