cmake_minimum_required(VERSION 3.18)

#set(CMAKE_CXX_COMPILER g++-12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(pimpl)

include(FetchContent)
FetchContent_Declare(lexy URL https://github.com/foonathan/lexy/releases/download/v2022.12.1/lexy-src.zip)
FetchContent_MakeAvailable(lexy)

include_directories(include)
include_directories(3rd_party/cli/include)
add_subdirectory(3rd_party/doctest)

set(pimpl_srcs
    src/parsing.cpp
)

add_library(pimpl ${pimpl_srcs})
target_include_directories(pimpl PUBLIC include)
target_link_libraries(pimpl PRIVATE foonathan::lexy doctest)

add_subdirectory(app)
add_subdirectory(test)
