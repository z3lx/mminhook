cmake_minimum_required(VERSION 3.28)
project(mminhook)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_COMMIT v1.3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(minhook)

add_executable(mminhook)

file(GLOB_RECURSE SOURCES src/*)
target_sources(mminhook PRIVATE ${SOURCES})
target_include_directories(mminhook PRIVATE include)
target_link_libraries(mminhook PUBLIC minhook)
