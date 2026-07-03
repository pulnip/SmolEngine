add_executable(SmolApp)

file(GLOB ALL_SOURCES
    "*.cpp"
)

target_sources(SmolApp
PRIVATE
    ${ALL_SOURCES}
)

target_include_directories(SmolApp
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_options(SmolApp
PRIVATE
    ${RENDER_BACKEND_LINK_OPTIONS}
)
target_link_libraries(SmolApp
PRIVATE
    Smol::Core
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,Smol::Gameplay>"
    Smol::Input
    Smol::Log
    Smol::Platform
    Smol::Render
    Smol::Resource
    # Smol::Scene
    Smol::UI
    ${RENDER_BACKENDS}
    ${CANVAS_BACKENDS}
)
