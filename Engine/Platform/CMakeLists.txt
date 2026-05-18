smol_declare_module(Platform)

target_link_libraries(SmolPlatform
PUBLIC
    Smol::Core
    Smol::Log
    Smol::Serialization
    Smol::RHI
PRIVATE
    SDL3::SDL3
    imgui
)
