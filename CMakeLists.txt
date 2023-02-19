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

#set(pimple_srcs)

add_library(pimpl INTERFACE)
target_include_directories(pimpl INTERFACE include)
target_link_libraries(pimpl INTERFACE foonathan::lexy)

add_subdirectory(app)
add_subdirectory(test)
