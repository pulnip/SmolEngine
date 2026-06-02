function(smol_serialization_sample NAME)
    cmake_parse_arguments(ARG "" "SOURCES" "DEPENDS" ${ARGN})

    add_executable(Smol${NAME}Sample ${ARG_SOURCES})

    target_link_libraries(Smol${NAME}Sample
    PRIVATE
        Smol::Serialization
        ${ARG_DEPENDS}
    )
endfunction()

smol_serialization_sample(ImageLoader
    SOURCES ImageLoaderSample.cpp
)
