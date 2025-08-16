cmake_minimum_required(VERSION 3.28.2)
project(mminhook
    LANGUAGES CXX
    VERSION 1.0.0
)

include(FetchContent)
include(GenerateExportHeader)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG v1.3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(minhook)

add_library(mminhook)
set(GENERATED_INCLUDE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)
set(GENERATED_DEFINES_HPP
    "${GENERATED_INCLUDE_DIR}/mmh/detail/GeneratedDefines.hpp"
)
generate_export_header(mminhook
    BASE_NAME mmh
    EXPORT_FILE_NAME "${GENERATED_DEFINES_HPP}"
    EXPORT_MACRO_NAME MMH_API
)
target_sources(mminhook
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            "${GENERATED_INCLUDE_DIR}"
        FILES
            include/mmh/Error.hpp
            include/mmh/Exception.hpp
            include/mmh/Hook.hpp
            include/mmh/detail/Defines.hpp
            include/mmh/detail/HookImpl.hpp
            include/mmh/detail/MinHook.hpp
            "${GENERATED_DEFINES_HPP}"
        FILE_SET CXX_MODULES
        FILES
            src/mmh.ixx
    PRIVATE
        src/mmh/Error.cpp
        src/mmh/Exception.cpp
        src/mmh/detail/MinHook.cpp
)
target_link_libraries(mminhook PRIVATE minhook)
target_compile_features(mminhook PUBLIC cxx_std_23)
set_target_properties(mminhook PROPERTIES
    CXX_EXTENSIONS FALSE
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
)

add_executable(demo src/main.cpp)
target_link_libraries(demo PRIVATE mminhook)
