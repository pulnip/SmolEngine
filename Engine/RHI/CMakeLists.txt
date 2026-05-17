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

if(RENDER_BACKEND STREQUAL "DX11")
    target_compile_definitions(SmolRHI
    PUBLIC
        SMOL_DXRHI
        SMOL_DX11RHI
    )
elseif(RENDER_BACKEND STREQUAL "Metal")
    target_compile_definitions(SmolRHI
    PUBLIC
        SMOL_METALRHI
    )
endif()

# if(SMOL_ENABLE_TEST)
#     add_subdirectory(Sample)

#     smol_declare_test(RHI
#         DIRECTORY Test
#         DEPENDS ${RENDER_BACKEND_TARGET}
#         LABELS "unit"
#     )
# endif()