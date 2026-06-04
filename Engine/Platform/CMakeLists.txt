smol_declare_module(Platform)

target_link_libraries(SmolPlatform
PUBLIC
    Smol::Core
    Smol::Serialization
    Smol::RHI
PRIVATE
    SDL3::SDL3
    imgui
    Smol::Log
)

smol_declare_private_interface(Platform)

if(SMOL_ENABLE_TEST)
    smol_declare_test(Platform
        DIRECTORY Test
        DEPENDS Smol::Platform
        LABELS "unit"
    )
endif()
