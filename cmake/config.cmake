set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# OS config
if(WIN32)
    set(RENDER_BACKEND "DX11" CACHE STRING "Rendering backend")
    if(RENDER_BACKEND STREQUAL "Metal")
        message(FATAL_ERROR "Metal is not supported on Windows.")
    endif()

    if(MSVC)
        add_compile_options(/EHsc)
        add_compile_definitions(NOMINMAX)
    endif()
elseif(APPLE)
    set(RENDER_BACKEND "Metal" CACHE STRING "Rendering backend")
    if(RENDER_BACKEND STREQUAL "DX11")
        message(FATAL_ERROR "DX11 is not supported on macOS.")
    endif()

    execute_process(
        COMMAND xcrun --sdk macosx --show-sdk-path
        OUTPUT_VARIABLE MACOSX_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_SYSROOT "${MACOSX_SDK_PATH}")
endif()

set_property(CACHE RENDER_BACKEND PROPERTY STRINGS "Metal" "DX11")

# Graphics API config
if(RENDER_BACKEND STREQUAL "DX11")
    file(GLOB WINDOWS_SDK_BIN_PATHS
        "$ENV{WindowsSdkDir}/bin/*/x64"
        "C:/Program Files (x86)/Windows Kits/10/bin/*/x64"
    )
    list(SORT WINDOWS_SDK_BIN_PATHS ORDER DESCENDING)

    find_program(SHADER_COMPILER NAMES fxc HINTS
        "$ENV{VCToolsInstallDir}/bin/Hostx64/x64"
        ${WINDOWS_SDK_BIN_PATHS}
    )
    if(NOT SHADER_COMPILER)
        message(FATAL_ERROR
            "fxc.exe (HLSL Shader Compiler) not found!"
            "Please check your Windows SDK installation."
        )
    endif()
elseif(RENDER_BACKEND STREQUAL "Metal")
    find_library(METAL_LIBRARY Metal REQUIRED)
    find_library(METALKIT_LIBRARY MetalKit REQUIRED)
    find_library(QUARTZCORE_LIBRARY QuartzCore REQUIRED)
    find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
else()
    message(FATAL_ERROR "Unsupported RENDER_BACKEND: ${RENDER_BACKEND}")
endif()

include(deps)
include(util)

if(SMOL_ENABLE_TEST)
    enable_testing()
    include(CTest)
    include(GoogleTest)
endif()