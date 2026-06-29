set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# Compiler Config
add_library(SmolProjectInterface INTERFACE)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
        # clang-cl config
        # use Exception Handling for stack unwinding
        add_compile_options(
            /EHsc
        )
        add_compile_definitions(
            NOMINMAX
            VC_EXTRALEAN
            WIN32_LEAN_AND_MEAN
        )
        target_compile_options(SmolProjectInterface
        INTERFACE
            /utf-8
        )
    else()
        # pure Clang or AppleClang config here
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(
        /EHsc
        /MP
        /Zc:preprocessor
    )
    add_compile_definitions(
        NOMINMAX
        VC_EXTRALEAN
        WIN32_LEAN_AND_MEAN
    )
    target_compile_options(SmolProjectInterface
    INTERFACE
        /utf-8
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(SmolProjectInterface
    INTERFACE
        -finput-charset=UTF-8
    )
endif()

# OS config
if(WIN32)
    # find HLSL compiler
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

    set(RENDER_BACKENDS Smol::DX11RHI)
    set(CANVAS_BACKENDS Smol::D2DCanvas Smol::ImGuiCanvas)
elseif(APPLE)
    execute_process(
        COMMAND xcrun --sdk macosx --show-sdk-path
        OUTPUT_VARIABLE MACOSX_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_SYSROOT "${MACOSX_SDK_PATH}")

    find_library(METAL_LIBRARY Metal REQUIRED)
    find_library(METALKIT_LIBRARY MetalKit REQUIRED)
    find_library(QUARTZCORE_LIBRARY QuartzCore REQUIRED)
    find_library(FOUNDATION_LIBRARY Foundation REQUIRED)

    set(RENDER_BACKENDS Smol::MetalRHI)
    set(CANVAS_BACKENDS Smol::ImGuiCanvas)
endif()

include(deps)
include(util)

if(SMOL_ENABLE_TEST)
    enable_testing()
    include(CTest)
    include(GoogleTest)
endif()