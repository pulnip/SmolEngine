function(smol_rhi_sample NAME)
    cmake_parse_arguments(ARG "" "SOURCES" "DEPENDS" ${ARGN})

    add_executable(${NAME}Sample ${ARG_SOURCES})

    target_link_libraries(${NAME}Sample
    PRIVATE
        ${RENDER_BACKEND_TARGET}
        Smol::Platform
        Smol::Log
        ${ARG_DEPENDS}
    )

    smol_rhi_macro(${NAME}Sample)
endfunction()

smol_rhi_sample(Texture
SOURCES TextureSample.cpp
DEPENDS
    Smol::Serialization
    SDL3::SDL3
    Smol::Platform::Private
)
