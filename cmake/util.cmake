# symlink for Config/ or Content/ directory
function(smol_link_or_copy_directory DIR_NAME)
    set(SRC "${CMAKE_SOURCE_DIR}/${DIR_NAME}")
    set(DST "${CMAKE_BINARY_DIR}/bin/${DIR_NAME}")

    if(EXISTS "${DST}")
        return()
    endif()

    if(WIN32)
        file(TO_NATIVE_PATH "${DST}" DST_NATIVE)
        file(TO_NATIVE_PATH "${SRC}" SRC_NATIVE)
        execute_process(
            COMMAND cmd /c mklink /J "${DST_NATIVE}" "${SRC_NATIVE}"
            RESULT_VARIABLE LINK_RESULT
        )
    else()
        file(CREATE_LINK "${SRC}" "${DST}"
            RESULT LINK_RESULT
            SYMBOLIC
        )
    endif()

    # fallback: copy directory
    if(NOT LINK_RESULT EQUAL 0)
        message(STATUS "[${DIR_NAME}] Symlink failed, falling back to copy...")
        file(COPY "${SRC}" DESTINATION "${CMAKE_BINARY_DIR}/bin")
    endif()
endfunction()

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
smol_link_or_copy_directory(Content)
smol_link_or_copy_directory(Config)

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

    target_link_libraries(Smol${NAME}
    PUBLIC
        SmolProjectInterface
    )

    add_library(Smol::${NAME} ALIAS Smol${NAME})
endfunction()

function(smol_declare_private_interface NAME)
    add_library(Smol${NAME}Private INTERFACE)

    target_include_directories(Smol${NAME}Private
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/Private"
    )

    target_link_libraries(Smol${NAME}Private
    INTERFACE
        Smol::${NAME}
    )

    add_library(Smol::${NAME}::Private ALIAS Smol${NAME}Private)
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

    target_link_libraries(Smol${NAME}
    INTERFACE
        SmolProjectInterface
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
        NAME Smol${NAME}Test
        COMMAND $<TARGET_FILE:Smol${NAME}Test>
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
    set_tests_properties(Smol${NAME}Test
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
            SMOL_DXRHI
            SMOL_DX11RHI
        )
    endif()
endfunction()
