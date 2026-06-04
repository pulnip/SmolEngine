add_library(SmolEngineSampleLib STATIC)

target_sources(SmolEngineSampleLib
PRIVATE
    SimplePlayer.cpp
)

target_link_libraries(SmolEngineSampleLib
PUBLIC
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,Smol::Gameplay>"
)

function(smol_engine_sample NAME)
    cmake_parse_arguments(ARG "" "SOURCES" "DEPENDS" ${ARGN})

    add_executable(Smol${NAME}Sample ${ARG_SOURCES})

    target_link_libraries(Smol${NAME}Sample
    PRIVATE
        ${RENDER_BACKEND_TARGET}
        "$<LINK_LIBRARY:WHOLE_ARCHIVE,SmolEngineSampleLib>"
        Smol::Gameplay
        Smol::Input
        Smol::Platform
        Smol::Render
        ${ARG_DEPENDS}
    )
endfunction()

smol_engine_sample(ActorUpdate
SOURCES ActorUpdateSample.cpp
DEPENDS
    SDL3::SDL3
    Smol::Platform::Private
)
