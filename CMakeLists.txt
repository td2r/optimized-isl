cmake_minimum_required(VERSION 3.23)
project(optimized_interval_skip_list)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules/)
option(CODE_COVERAGE "Generate code coverage info" OFF)
set(SANITIZER_OPTIONS
    -fsanitize=address -fno-omit-frame-pointer
    -fsanitize=undefined
    -fsanitize=leak
    -fsanitize=pointer-compare
)

add_executable(main
    include/Interval_skip_list.h
    include/Interval_skip_list_interval.h
    main.cpp
    include/Interval_cartesian_tree.h)
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
    include/Interval_cartesian_tree.h
    tests/interval_skip_list_test.cc
)
target_link_libraries(
    interval_skip_list_test
    GTest::gtest_main
)
target_compile_options(interval_skip_list_test PRIVATE ${SANITIZER_OPTIONS})
target_link_options(interval_skip_list_test PRIVATE ${SANITIZER_OPTIONS})
include(GoogleTest)
gtest_discover_tests(interval_skip_list_test)

find_package(benchmark REQUIRED)
add_executable(isl_benchmark
    utils/utils.h
    include/Interval_cartesian_tree.h
    benchmark/benchmark.cc
)
target_link_libraries(isl_benchmark benchmark::benchmark)
target_compile_options(isl_benchmark PRIVATE "-O3")

add_executable(memory_usage
    utils/utils.h
    include/Interval_skip_list.h
    include/Interval_cartesian_tree.h
    include/Interval_skip_list_interval.h
    valgrind/memory_usage.cpp
)

########################################################################################################################
##########################################TEST_COVERAGE#################################################################
########################################################################################################################

if (CMAKE_BUILD_TYPE MATCHES Debug)
    if (CODE_COVERAGE MATCHES ON)
        include(CodeCoverage)
        set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage ${CMAKE_CXX_FLAGS}")
        setup_target_for_coverage(test_coverage interval_skip_list_test coverage)
    endif()
endif()
