add_library(SmolGameplaySampleLib STATIC)

target_sources(SmolGameplaySampleLib
PRIVATE
    TestPawn.cpp
)

target_link_libraries(SmolGameplaySampleLib
PUBLIC
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,Smol::Gameplay>"
)

function(smol_gameplay_sample NAME)
    cmake_parse_arguments(ARG "" "SOURCES" "DEPENDS" ${ARGN})

    add_executable(Smol${NAME}Sample ${ARG_SOURCES})

    target_link_libraries(Smol${NAME}Sample
    PRIVATE
        "$<LINK_LIBRARY:WHOLE_ARCHIVE,SmolGameplaySampleLib>"
        ${ARG_DEPENDS}
    )
endfunction()

smol_gameplay_sample(CharacterController
SOURCES CharacterControllerSample.cpp
DEPENDS
    SDL3::SDL3
    Smol::Platform::Private
)
