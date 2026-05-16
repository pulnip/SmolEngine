smol_declare_module(Platform)

target_link_libraries(SmolPlatform
PUBLIC
    Smol::Core
    Smol::Serialization
PRIVATE
    SDL3::SDL3
    imgui
)
