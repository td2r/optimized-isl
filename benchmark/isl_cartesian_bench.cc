#include "benchmarks.h"

#include <benchmark/benchmark.h>

#include "../include/Interval_skip_list.h"
#include "../include/Interval_cartesian_tree.h"
#include "../include/Interval_skip_list_interval.h"

static const int INSERT_N = 100000;
static const int64_t INSERT_ITERATIONS = 100;
static const benchmark::TimeUnit INSERT_TIME_UNIT = benchmark::kMicrosecond;

BENCHMARK(BM_Insert<Interval_skip_list_interval<double>, Interval_skip_list, Sparse_data>)
    ->Name("InsertSparseISL")
    ->Apply(DecimalArgs<INSERT_N>)
    ->Iterations(INSERT_ITERATIONS)
    ->Unit(INSERT_TIME_UNIT);

BENCHMARK(BM_Insert<Interval_skip_list_interval<double>, Interval_cartesian_tree, Sparse_data>)
   ->Name("InsertSparseCartesian")
   ->Apply(DecimalArgs<INSERT_N>)
   ->Iterations(INSERT_ITERATIONS)
   ->Unit(INSERT_TIME_UNIT);

BENCHMARK(BM_Insert<Interval_skip_list_interval<double>, Interval_skip_list, Dense_data>)
    ->Name("InsertDenseISL")
    ->Apply(DecimalArgs<INSERT_N>)
    ->Iterations(INSERT_ITERATIONS)
    ->Unit(INSERT_TIME_UNIT);

BENCHMARK(BM_Insert<Interval_skip_list_interval<double>, Interval_cartesian_tree, Dense_data>)
   ->Name("InsertDenseCartesian")
   ->Apply(DecimalArgs<INSERT_N>)
   ->Iterations(INSERT_ITERATIONS)
   ->Unit(INSERT_TIME_UNIT);

BENCHMARK(BM_Insert<Interval_skip_list_interval<double>, Interval_skip_list, Random_data>)
    ->Name("InsertRandomISL")
    ->Apply(DecimalArgs<INSERT_N>)
    ->Iterations(INSERT_ITERATIONS)
    ->Unit(INSERT_TIME_UNIT);

BENCHMARK(BM_Insert<Interval_skip_list_interval<double>, Interval_cartesian_tree, Random_data>)
   ->Name("InsertRandomCartesian")
   ->Apply(DecimalArgs<INSERT_N>)
   ->Iterations(INSERT_ITERATIONS)
   ->Unit(INSERT_TIME_UNIT);

static const int DELETE_N = 100000;
static const uint64_t DELETE_ITERATIONS = 100;
static const benchmark::TimeUnit DELETE_TIME_UNIT = benchmark::kMicrosecond;

BENCHMARK(BM_Delete<Interval_skip_list_interval<double>, Interval_skip_list, Sparse_data>)
    ->Name("DeleteSparseISL")
    ->Apply(DecimalArgs<DELETE_N>)
    ->Iterations(DELETE_ITERATIONS)
    ->Unit(DELETE_TIME_UNIT);

BENCHMARK(BM_Delete<Interval_skip_list_interval<double>, Interval_cartesian_tree, Sparse_data>)
    ->Name("DeleteSparseCartesian")
    ->Apply(DecimalArgs<DELETE_N>)
    ->Iterations(DELETE_ITERATIONS)
    ->Unit(DELETE_TIME_UNIT);

BENCHMARK(BM_Delete<Interval_skip_list_interval<double>, Interval_skip_list, Dense_data>)
    ->Name("DeleteDenseISL")
    ->Apply(DecimalArgs<DELETE_N>)
    ->Iterations(DELETE_ITERATIONS)
    ->Unit(DELETE_TIME_UNIT);

BENCHMARK(BM_Delete<Interval_skip_list_interval<double>, Interval_cartesian_tree, Dense_data>)
    ->Name("DeleteDenseCartesian")
    ->Apply(DecimalArgs<DELETE_N>)
    ->Iterations(DELETE_ITERATIONS)
    ->Unit(DELETE_TIME_UNIT);

BENCHMARK(BM_Delete<Interval_skip_list_interval<double>, Interval_skip_list, Random_data>)
    ->Name("DeleteRandomISL")
    ->Apply(DecimalArgs<DELETE_N>)
    ->Iterations(DELETE_ITERATIONS)
    ->Unit(DELETE_TIME_UNIT);

BENCHMARK(BM_Delete<Interval_skip_list_interval<double>, Interval_cartesian_tree, Random_data>)
    ->Name("DeleteRandomCartesian")
    ->Apply(DecimalArgs<DELETE_N>)
    ->Iterations(DELETE_ITERATIONS)
    ->Unit(DELETE_TIME_UNIT);

static const int SEARCH_N = 100000;
static const uint64_t SEARCH_ITERATIONS = 100;
static const benchmark::TimeUnit SEARCH_TIME_UNIT = benchmark::kMicrosecond;

BENCHMARK(BM_Search<Interval_skip_list_interval<double>, Interval_skip_list, Sparse_data>)
    ->Name("SearchSparseISL")
    ->Apply(DecimalArgs<SEARCH_N>)
    ->Iterations(SEARCH_ITERATIONS)
    ->Unit(SEARCH_TIME_UNIT);

BENCHMARK(BM_Search<Interval_skip_list_interval<double>, Interval_cartesian_tree, Sparse_data>)
    ->Name("SearchSparseCartesian")
    ->Apply(DecimalArgs<SEARCH_N>)
    ->Iterations(SEARCH_ITERATIONS)
    ->Unit(SEARCH_TIME_UNIT);

BENCHMARK(BM_Search<Interval_skip_list_interval<double>, Interval_skip_list, Dense_data>)
    ->Name("SearchDenseISL")
    ->Apply(DecimalArgs<SEARCH_N>)
    ->Iterations(SEARCH_ITERATIONS)
    ->Unit(SEARCH_TIME_UNIT);

BENCHMARK(BM_Search<Interval_skip_list_interval<double>, Interval_cartesian_tree, Dense_data>)
    ->Name("SearchDenseCartesian")
    ->Apply(DecimalArgs<SEARCH_N>)
    ->Iterations(SEARCH_ITERATIONS)
    ->Unit(SEARCH_TIME_UNIT);

BENCHMARK(BM_Search<Interval_skip_list_interval<double>, Interval_skip_list, Random_data>)
    ->Name("SearchRandomISL")
    ->Apply(DecimalArgs<SEARCH_N>)
    ->Iterations(SEARCH_ITERATIONS)
    ->Unit(SEARCH_TIME_UNIT);

BENCHMARK(BM_Search<Interval_skip_list_interval<double>, Interval_cartesian_tree, Random_data>)
    ->Name("SearchRandomCartesian")
    ->Apply(DecimalArgs<SEARCH_N>)
    ->Iterations(SEARCH_ITERATIONS)
    ->Unit(SEARCH_TIME_UNIT);

BENCHMARK_MAIN();
