cmake_minimum_required(VERSION 3.5)

project(pimpl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(${PROJECT_NAME}
  src/types.cpp
  src/parsing.cpp
)

add_executable(main src/main.cpp)

