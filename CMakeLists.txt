cmake_minimum_required(VERSION 3.16)

project(pimpl)

#set(CMAKE_CXX_COMPILER g++-12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PIMPL_BUILD_TESTS "Should tests be built" ON)

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
target_compile_definitions(pimpl PRIVATE DOCTEST_CONFIG_DISABLE)

add_subdirectory(app)

if(PIMPL_BUILD_TESTS)
    add_executable(tests test/doctest_main.cpp ${pimpl_srcs})
    target_compile_definitions(tests PRIVATE PIMPL_ENABLE_TESTS)
    target_link_libraries(tests pimpl foonathan::lexy doctest)
endif()
