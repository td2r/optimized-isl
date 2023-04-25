cmake_minimum_required(VERSION 3.23)
project(optimized_interval_skip_list)

set(CMAKE_CXX_STANDARD 14)

add_executable(main
    include/Interval_skip_list.h
    include/Interval_skip_list_interval.h
    main.cpp
)
target_compile_options(main PRIVATE "-O3")
target_compile_definitions(main PRIVATE CGAL_DISABLE_ROUNDING_MATH_CHECK=ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(interval_skip_list_test
    include/Interval_skip_list_interval.h
    include/Interval_skip_list.h
    tests/interval_skip_list_test.cc
)
target_link_libraries(
    interval_skip_list_test
    GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(interval_skip_list_test)

find_package(benchmark REQUIRED)
add_executable(isl_benchmark
    utils/utils.h
    benchmark/benchmark.cc
)
target_link_libraries(isl_benchmark benchmark::benchmark)
target_compile_options(isl_benchmark PRIVATE "-O3")

add_executable(memory_usage
    include/Interval_skip_list.h
    include/Interval_skip_list_interval.h
    utils/utils.h
    valgrind/memory_usage.cpp
)
