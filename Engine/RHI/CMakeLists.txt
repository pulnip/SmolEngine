smol_declare_interface(CommonRHI
    DIRECTORY Common
)

smol_declare_module(RHI)

target_link_libraries(SmolRHI
PUBLIC
    Smol::CommonRHI
    Smol::Core
PRIVATE
    Smol::Log
)

if(SMOL_ENABLE_TEST)
    add_subdirectory(Sample)
endif()
