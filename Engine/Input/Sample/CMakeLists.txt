function(smol_input_sample NAME)
    cmake_parse_arguments(ARG "" "SOURCES" "DEPENDS" ${ARGN})

    add_executable(Smol${NAME}Sample ${ARG_SOURCES})

    target_link_libraries(Smol${NAME}Sample
    PRIVATE
        Smol::Input
        ${ARG_DEPENDS}
    )
endfunction()

smol_input_sample(InputManager
    SOURCES InputManagerSample.cpp
    DEPENDS SDL3::SDL3
)
