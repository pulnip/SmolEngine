include(FetchContent)

find_package(SDL3 QUIET)
if(NOT SDL3_FOUND)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "release-3.2.30"
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    # Library Type
    set(SDL_SHARED ON  CACHE BOOL "" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    # Feature
    set(SDL_AUDIO    OFF CACHE BOOL "" FORCE)
    set(SDL_VIDEO    ON  CACHE BOOL "" FORCE)
    set(SDL_GPU      OFF CACHE BOOL "" FORCE)
    set(SDL_OPENGL   OFF CACHE BOOL "" FORCE)
    set(SDL_OPENGLES OFF CACHE BOOL "" FORCE)
    set(SDL_VULKAN   OFF CACHE BOOL "" FORCE)
    set(SDL_RENDER   OFF CACHE BOOL "" FORCE)
    set(SDL_CAMERA   OFF CACHE BOOL "" FORCE)
    set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
    set(SDL_HAPTIC   OFF CACHE BOOL "" FORCE)
    set(SDL_HIDAPI   OFF CACHE BOOL "" FORCE)
    set(SDL_POWER    OFF CACHE BOOL "" FORCE)
    set(SDL_SENSOR   OFF CACHE BOOL "" FORCE)
    set(SDL_DIALOG   OFF CACHE BOOL "" FORCE)
    # Test Build
    set(SDL_TESTS         OFF CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY  OFF CACHE BOOL "" FORCE)
    # Install
    set(SDL_INSTALL       OFF CACHE BOOL "" FORCE)
    set(SDL_DISABLE_INSTALL_DOCS ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL3)
    if(TARGET SDL3-shared)
        target_compile_options(SDL3-shared PRIVATE
            $<$<C_COMPILER_ID:Clang,AppleClang,GNU>:-Wno-deprecated-declarations>
        )
    elseif(TARGET SDL3-static)
        target_compile_options(SDL3-static PRIVATE
            $<$<C_COMPILER_ID:Clang,AppleClang,GNU>:-Wno-deprecated-declarations>
        )
    endif()
endif()

# stb - header-only image loading library
FetchContent_Declare(
    stb
    GIT_REPOSITORY "https://github.com/nothings/stb.git"
    GIT_TAG "31c1ad37456438565541f4919958214b6e762fb4"
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(stb)

add_library(stb INTERFACE)

target_include_directories(stb
SYSTEM INTERFACE
    "${stb_SOURCE_DIR}"
)

# tomlplusplus - TOML parser library
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY "https://github.com/marzer/tomlplusplus.git"
    GIT_TAG "v3.4.0"
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(tomlplusplus)

# Metal-cpp
if(RENDER_BACKEND STREQUAL "Metal")
    FetchContent_Declare(
        metal-cpp
        URL https://developer.apple.com/metal/cpp/files/metal-cpp_macOS15_iOS18.zip
    )
    FetchContent_MakeAvailable(metal-cpp)
endif()

# Dear ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        "v1.92.5"
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC)

target_sources(imgui
PUBLIC
    ${imgui_SOURCE_DIR}/imgui.h
PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    $<$<BOOL:${SMOL_ENABLE_TEST}>:${imgui_SOURCE_DIR}/imgui_demo.cpp>
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
)

target_include_directories(imgui
PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui
PRIVATE
    SDL3::SDL3
)

if(RENDER_BACKEND STREQUAL "Metal")
    target_compile_options(imgui
    PRIVATE
        $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wno-deprecated-declarations>
        $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wno-arc-bridge-casts-disallowed-in-nonarc>
    )

    target_compile_definitions(imgui
    PUBLIC
        IMGUI_IMPL_METAL_CPP
    )

    target_sources(imgui
    PRIVATE
        ${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm
    )
    target_include_directories(imgui
    SYSTEM PUBLIC
        "${metal-cpp_SOURCE_DIR}"
    )
    target_link_libraries(imgui
    PRIVATE
        ${METAL_LIBRARY}
    )
elseif(RENDER_BACKEND STREQUAL "DX11")
    target_sources(imgui
    PRIVATE
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
    )
endif()

if(SMOL_ENABLE_TEST)
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        FetchContent_Declare(
            GTest
            GIT_REPOSITORY "https://github.com/google/googletest.git"
            GIT_TAG "v1.17.0"
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
        )
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(GTEST_HAS_ABSL OFF CACHE BOOL "" FORCE)
        add_compile_options(
            $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wno-character-conversion>
        )
        FetchContent_MakeAvailable(GTest)
    endif()
endif()
