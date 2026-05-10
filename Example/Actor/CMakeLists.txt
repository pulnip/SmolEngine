file(GLOB ALL_HEADERS
    "*.hpp"
)

file(GLOB ALL_SOURCES
    "*.cpp"
)

add_library(ExampleActor)

target_sources(ExampleActor
PUBLIC
    ${ALL_HEADERS}
PRIVATE
    ${ALL_SOURCES}
)

target_include_directories(ExampleActor
PUBLIC
    ${ALL_HEADERS}
)

target_link_libraries(ExampleActor
PRIVATE
    SmolCore
)
