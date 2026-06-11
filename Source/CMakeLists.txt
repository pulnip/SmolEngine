file(GLOB ALL_HEADERS
    "Sample/*.hpp"
    "*.hpp"
)

file(GLOB ALL_SOURCES
    "Sample/*.cpp"
    "*.cpp"
)

add_library(MyGame STATIC)

target_sources(MyGame
PUBLIC
    ${ALL_HEADERS}
PRIVATE
    ${ALL_SOURCES}
)

target_link_libraries(MyGame
PUBLIC
    Smol::Gameplay
PRIVATE
    Smol::Log
)

target_link_libraries(SmolApp
PRIVATE
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,MyGame>"
)

add_subdirectory(Example)
