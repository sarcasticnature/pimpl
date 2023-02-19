set(test_srcs
    doctest_main.cpp
    parsing.cpp
)
add_executable(tests ${test_srcs})

target_link_libraries(tests pimpl doctest foonathan::lexy)
