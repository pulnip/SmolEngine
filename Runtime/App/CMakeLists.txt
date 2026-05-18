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
    Smol::Render
    Smol::Scene
)

if(RENDER_BACKEND STREQUAL "DX11")
    target_link_libraries(SmolApp
    PRIVATE
        Smol::DX11RHI
    )
elseif(RENDER_BACKEND STREQUAL "Metal")
    target_link_libraries(SmolApp
    PRIVATE
        Smol::MetalRHI
    )
endif()
