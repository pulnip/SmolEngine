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

target_link_libraries(SmolApp
PRIVATE
    Smol::Core
    Smol::Platform
    Smol::Scene
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,ExampleActor>"
)
