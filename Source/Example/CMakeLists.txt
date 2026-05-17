file(GLOB ALL_HEADERS
    "*.hpp"
)

file(GLOB ALL_SOURCES
    "*.cpp"
)

add_library(ExampleLib)

target_sources(ExampleLib
PUBLIC
    ${ALL_HEADERS}
PRIVATE
    ${ALL_SOURCES}
)

target_include_directories(ExampleLib
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ExampleLib
PUBLIC
    Smol::Core
    Smol::Reflection
PRIVATE
)
