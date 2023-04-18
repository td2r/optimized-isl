cmake_minimum_required(VERSION 3.23)
project(optimized_interval_skip_list)

set(CMAKE_CXX_STANDARD 14)

add_executable(optimized_interval_skip_list
    main.cpp
    include/Interval_skip_list.h
    include/Interval_skip_list_interval.h
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    interval_skip_list_test
    tests/interval_skip_list_test.cc
    include/Interval_skip_list_interval.h
    include/Interval_skip_list.h
)
target_link_libraries(
    interval_skip_list_test
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(interval_skip_list_test)

