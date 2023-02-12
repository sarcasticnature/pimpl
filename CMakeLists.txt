cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_COMPILER g++-12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(pimpl)

include_directories(include)

#set(pimple_srcs)

add_library(pimpl INTERFACE)
target_include_directories(pimpl INTERFACE include)

add_subdirectory(examples)

