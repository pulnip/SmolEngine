add_library(SmolGameplay STATIC)

file(GLOB PUBLIC_SOURCES
    "Public/*.hpp"
)
file(GLOB PUBLIC_ACTOR_SOURCES
    "Public/Actor/*.hpp"
)
file(GLOB PUBLIC_COMPONENT_SOURCES
    "Public/Component/*.hpp"
)
file(GLOB PRIVATE_SOURCES
    "Private/*.hpp"
    "Private/*.cpp"
)
file(GLOB PRIVATE_ACTOR_SOURCES
    "Private/Actor/*.hpp"
    "Private/Actor/*.cpp"
)
file(GLOB PRIVATE_COMPONENT_SOURCES
    "Private/Component/*.hpp"
    "Private/Component/*.cpp"
)

target_sources(SmolGameplay
PUBLIC
    ${PUBLIC_SOURCES}
    ${PUBLIC_ACTOR_SOURCES}
    ${PUBLIC_COMPONENT_SOURCES}
PRIVATE
    ${PRIVATE_SOURCES}
    ${PRIVATE_ACTOR_SOURCES}
    ${PRIVATE_COMPONENT_SOURCES}
)

target_include_directories(SmolGameplay
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Public"
    "${CMAKE_CURRENT_SOURCE_DIR}/Public/Actor"
    "${CMAKE_CURRENT_SOURCE_DIR}/Public/Component"
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Private"
    "${CMAKE_CURRENT_SOURCE_DIR}/Private/Actor"
    "${CMAKE_CURRENT_SOURCE_DIR}/Private/Component"
)

target_link_libraries(SmolGameplay
PUBLIC
    Smol::Core
    Smol::Reflection
    Smol::Input
    Smol::Render
PRIVATE
    Smol::Log
)

add_library(Smol::Gameplay ALIAS SmolGameplay)

if(SMOL_ENABLE_TEST)
    add_subdirectory(Sample)
endif()
