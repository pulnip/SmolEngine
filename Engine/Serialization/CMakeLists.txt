smol_declare_module(Serialization)

target_compile_definitions(SmolSerialization
PRIVATE
    TOML_EXCEPTIONS=1
)

target_include_directories(SmolSerialization
SYSTEM PRIVATE
    "${stb_SOURCE_DIR}"
)

target_link_libraries(SmolSerialization
PUBLIC
    Smol::Core
PRIVATE
    tomlplusplus::tomlplusplus
)

smol_declare_private_interface(Serialization)

if(SMOL_ENABLE_TEST)
    smol_declare_test(Serialization
        DIRECTORY Test
        DEPENDS SmolSerialization_Private
        LABELS "unit"
    )
endif()
