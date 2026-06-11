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
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,Smol::Gameplay>"
    Smol::Input
    Smol::Log
    Smol::Platform
    Smol::Render
    Smol::Resource
    # Smol::Scene
)

if(RENDER_BACKEND STREQUAL "DX11")
    target_link_libraries(SmolApp
    PRIVATE
        Smol::DX11RHI
        Smol::D2DCanvas
    )
elseif(RENDER_BACKEND STREQUAL "Metal")
    target_link_libraries(SmolApp
    PRIVATE
        Smol::MetalRHI
    )
endif()
