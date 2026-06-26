function(smol_render_sample NAME)
    cmake_parse_arguments(ARG "" "SOURCES" "DEPENDS" ${ARGN})

    add_executable(${NAME}Sample ${ARG_SOURCES})

    target_link_libraries(${NAME}Sample
    PRIVATE
        ${RENDER_BACKEND_TARGET}
        Smol::Render
        Smol::Platform
        Smol::Log
        ${ARG_DEPENDS}
    )

    smol_rhi_macro(${NAME}Sample)
endfunction()

smol_render_sample(SpriteRenderer
SOURCES SpriteRendererSample.cpp
DEPENDS
    SDL3::SDL3
    Smol::Platform::Private
)

smol_render_sample(SpriteRendererAnim
SOURCES SpriteRendererAnimSample.cpp
DEPENDS
    SDL3::SDL3
    Smol::Platform::Private
)

smol_render_sample(ParallaxMountains
SOURCES ParallaxMountains.cpp
DEPENDS
    SDL3::SDL3
    Smol::Platform::Private
)
