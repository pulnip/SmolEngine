function(smol_rhi_sample NAME)
    cmake_parse_arguments(ARG "" "SOURCES" "DEPENDS" ${ARGN})

    add_executable(${NAME}Sample ${ARG_SOURCES})

    target_link_libraries(${NAME}Sample
    PRIVATE
        Smol::Platform
        Smol::Log
        ${ARG_DEPENDS}
    )

    if(WIN32)
        target_link_libraries(${NAME}Sample
        PRIVATE
            Smol::DX11RHI
        )
    elseif(APPLE)
        target_link_libraries(${NAME}Sample
        PRIVATE
            Smol::MetalRHI
        )
    endif()
endfunction()

smol_rhi_sample(Texture
SOURCES TextureSample.cpp
DEPENDS
    Smol::Serialization
    SDL3::SDL3
    Smol::Platform::Private
)
