set(ASSET_SRC "${CMAKE_SOURCE_DIR}/Content")
set(ASSET_DST "${CMAKE_BINARY_DIR}/bin/Content")

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
if(NOT EXISTS "${ASSET_DST}")
    # try to link asset folder
    if(WIN32)
        file(TO_NATIVE_PATH "${ASSET_DST}" ASSET_DST_NATIVE)
        file(TO_NATIVE_PATH "${ASSET_SRC}" ASSET_SRC_NATIVE)
        execute_process(
            COMMAND cmd /c mklink /J "${ASSET_DST_NATIVE}" "${ASSET_SRC_NATIVE}"
            RESULT_VARIABLE LINK_RESULT
        )
    else()
        file(CREATE_LINK "${ASSET_SRC}" "${ASSET_DST}"
            RESULT LINK_RESULT
            SYMBOLIC
        )
    endif()

    # fallback: copy asset folder
    if(NOT LINK_RESULT EQUAL 0)
        message(STATUS "Symlink failed, falling back to copy...")
        file(COPY "${ASSET_SRC}" DESTINATION "${CMAKE_BINARY_DIR}/bin")
    endif()
endif()

function(smol_declare_module NAME)
    add_library(Smol${NAME} STATIC)

    file(GLOB PUBLIC_SOURCES
        "Public/*.hpp"
    )
    file(GLOB PRIVATE_SOURCES
        "Private/*.hpp"
        "Private/*.cpp"
    )

    target_sources(Smol${NAME}
    PUBLIC
        ${PUBLIC_SOURCES}
    PRIVATE
        ${PRIVATE_SOURCES}
    )

    target_include_directories(Smol${NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Public"
    )

    add_library(Smol::${NAME} ALIAS Smol${NAME})
endfunction()

function(smol_declare_private_interface NAME)
    add_library(Smol${NAME}_Private INTERFACE)

    target_include_directories(Smol${NAME}_Private INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/Private"
    )

    target_link_libraries(Smol${NAME}_Private INTERFACE Smol::${NAME})
endfunction()

function(smol_declare_interface NAME)
    cmake_parse_arguments(ARG "" "DIRECTORY" "" ${ARGN})

    add_library(Smol${NAME} INTERFACE)

    file(GLOB INTERFACE_SOURCES
        "${ARG_DIRECTORY}/*.hpp"
    )

    target_sources(Smol${NAME}
    INTERFACE
        ${INTERFACE_SOURCES}
    )

    target_include_directories(Smol${NAME}
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_DIRECTORY}"
    )

    add_library(Smol::${NAME} ALIAS Smol${NAME})
endfunction()

function(smol_declare_test NAME)
    cmake_parse_arguments(ARG "" "DIRECTORY" "LABELS;DEPENDS" ${ARGN})

    if(NOT ARG_DIRECTORY)
        set(ARG_DIRECTORY ".")
    endif()

    add_executable(Smol${NAME}Test)

    file(GLOB TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_DIRECTORY}/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_DIRECTORY}/*.cpp"
    )

    target_sources(Smol${NAME}Test
    PRIVATE
        ${TEST_SOURCES}
    )

    target_link_libraries(Smol${NAME}Test
    PRIVATE
        GTest::gtest_main
        GTest::gmock_main
        ${ARG_DEPENDS}
    )

    add_test(
        NAME Smol${NAME}-Test
        COMMAND $<TARGET_FILE:Smol${NAME}Test>
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
    set_tests_properties(Smol${NAME}-Test
    PROPERTIES
        LABELS "${ARG_LABELS}"
    )
endfunction()

function(smol_rhi_macro NAME)
    if(RENDER_BACKEND STREQUAL "Metal")
        target_compile_definitions(Smol${NAME}
        PRIVATE
            SMOL_METALRHI
        )
    elseif(RENDER_BACKEND STREQUAL "DX11")
        target_compile_definitions(Smol${NAME}
        PRIVATE
            SMOL_D3DRHI
            SMOL_DX11RHI
        )
    endif()
endfunction()