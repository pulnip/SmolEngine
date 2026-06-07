smol_declare_module(Resource)

target_link_libraries(SmolResource
PUBLIC
    Smol::Core
    Smol::RHI
PRIVATE
    Smol::Serialization
)
