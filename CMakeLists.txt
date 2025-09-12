cmake_minimum_required(VERSION 3.30)

option(MMH_BUILD_MODULES "Build mminhook as a C++20 module" ON)

if (MMH_BUILD_MODULES)
    if (NOT (CMAKE_GENERATOR STREQUAL "Ninja") AND
        NOT (CMAKE_GENERATOR STREQUAL "Ninja Multi-Config"))
        message(FATAL_ERROR
            "This project requires the Ninja generator to build C++20 modules."
        )
    endif()
    # https://github.com/Kitware/CMake/blob/v3.30.0/Help/dev/experimental.rst
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
        "0e5b6991-d74f-4b3d-a41c-cf096e0b2508"
    )
endif()

project(mminhook LANGUAGES CXX)

include(FetchContent)
include(GenerateExportHeader)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG v1.3.4
    GIT_SHALLOW TRUE
    EXCLUDE_FROM_ALL
)
set(BUILD_SHARED_LIBS FALSE)
FetchContent_MakeAvailable(minhook)
unset(BUILD_SHARED_LIBS)

set(MMH_GENERATED_INCLUDE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

set(MMH_GENERATED_DEFINES_HPP
    "${MMH_GENERATED_INCLUDE_DIR}/mmh/detail/GeneratedDefines.hpp"
)

set(MMH_HEADER_DIRS
    include
    "${MMH_GENERATED_INCLUDE_DIR}"
)

set(MMH_HEADER_FILES
    include/mmh/Error.hpp
    include/mmh/Exception.hpp
    include/mmh/Hook.hpp
    include/mmh/detail/Defines.hpp
    include/mmh/detail/HookImpl.hpp
    "${MMH_GENERATED_DEFINES_HPP}"
)

set(MMH_SOURCE_FILES
    src/Error.cpp
    src/Exception.cpp
    src/Hook.cpp
)

set(MODULE_FILES
    src/mmh.ixx
)

add_library(mminhook)
add_library(mmh::mmh ALIAS mminhook)

target_compile_features(mminhook PUBLIC cxx_std_23)
set_target_properties(mminhook PROPERTIES
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS FALSE
    CXX_SCAN_FOR_MODULES MMH_BUILD_MODULES
    CXX_MODULE_STD MMH_BUILD_MODULES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
)

generate_export_header(mminhook
    BASE_NAME mmh
    EXPORT_FILE_NAME "${MMH_GENERATED_DEFINES_HPP}"
    EXPORT_MACRO_NAME MMH_API
)

if (MMH_BUILD_MODULES)
    target_sources(mminhook
        PUBLIC
            FILE_SET CXX_MODULES
            FILES ${MODULE_FILES}
        PRIVATE
            FILE_SET HEADERS
            BASE_DIRS ${MMH_HEADER_DIRS}
            FILES ${MMH_HEADER_FILES}
        PRIVATE
            ${MMH_SOURCE_FILES}
    )
else()
    target_sources(mminhook
        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS ${MMH_HEADER_DIRS}
            FILES ${MMH_HEADER_FILES}
        PRIVATE
            ${MMH_SOURCE_FILES}
    )
endif()

target_link_libraries(mminhook PRIVATE minhook)
