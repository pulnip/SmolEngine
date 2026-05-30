smol_declare_module(MetalRHI)

target_include_directories(SmolMetalRHI
SYSTEM PRIVATE
    "${metal-cpp_SOURCE_DIR}"
)

target_link_libraries(SmolMetalRHI
PUBLIC
    Smol::CommonRHI
    Smol::RHI
PRIVATE
    ${METAL_LIBRARY}
    ${METALKIT_LIBRARY}
    ${QUARTZCORE_LIBRARY}
    ${FOUNDATION_LIBRARY}
    SDL3::SDL3
)
