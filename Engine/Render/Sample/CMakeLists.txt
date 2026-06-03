function(smol_render_sample NAME)
    cmake_parse_arguments(ARG "" "SOURCES" "DEPENDS" ${ARGN})

    add_executable(Smol${NAME}Sample ${ARG_SOURCES})

    target_link_libraries(Smol${NAME}Sample
    PRIVATE
        ${RENDER_BACKEND_TARGET}
        Smol::Render
        Smol::Platform
        Smol::Log
        ${ARG_DEPENDS}
    )
endfunction()

smol_render_sample(SpriteRenderer
    SOURCES SpriteRendererSample.cpp
)
