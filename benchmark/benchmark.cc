#include "../include/Interval_skip_list.h"
#include "../include/Interval_skip_list_interval.h"
#include "../utils/utils.h"

#include <CGAL/Interval_skip_list.h>
#include <CGAL/Interval_skip_list_interval.h>

#include <benchmark/benchmark.h>

#include <random>

template<class Interval_t, template<class> class ISL_t, template<class, template<class> class> class Data_t>
void BM_Insert(benchmark::State& st) {
  for (auto _ : st) {
    Data_t<Interval_t, ISL_t> data(st.range());
    // benchmark::DoNotOptimize(data);
    // data.isl.insert(data.interval);

    // st.PauseTiming();
    // data.isl.remove(data.interval);
    // data.set_new_interval();
    // st.ResumeTiming();
  }
  st.SetComplexityN(st.range(0));
}

static void InsertArguments(benchmark::internal::Benchmark* b) {
  for (int i = 10; i <= 100000; i *= 10) {
    b->Arg(i);
  }
  for (int i = 2; i <= 10; ++i) {
    b->Arg(100000 * i);
  }
}

BENCHMARK(BM_Insert<Interval_skip_list_interval<double>, Interval_skip_list, Sparse_data>)
    ->Name("InsertSparseISL")
    ->Apply(InsertArguments)
    ->Iterations(2)
    ->Unit(benchmark::kMicrosecond)
    ->Complexity();

BENCHMARK(BM_Insert<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list, Sparse_data>)
   ->Name("InsertSparseCGAL")
   ->Apply(InsertArguments)
   ->Iterations(10)
   ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Insert<Interval_skip_list_interval<double>, Interval_skip_list, Dense_data>)
    ->Name("InsertDenseISL")
    ->Apply(InsertArguments)
    ->Iterations(2)
    ->Unit(benchmark::kMicrosecond)
    ->Complexity();

BENCHMARK(BM_Insert<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list, Dense_data>)
   ->Name("InsertDenseCGAL")
   ->Apply(InsertArguments)
   ->Iterations(10)
   ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Insert<Interval_skip_list_interval<double>, Interval_skip_list, Random_data>)
    ->Name("InsertRandomISL")
    ->Apply(InsertArguments)
    ->Iterations(2)
    ->Unit(benchmark::kMicrosecond)
    ->Complexity();

BENCHMARK(BM_Insert<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list, Random_data>)
   ->Name("InsertRandomCGAL")
   ->Apply(InsertArguments)
   ->Iterations(10)
   ->Unit(benchmark::kMicrosecond);

template<class Interval_t, template<class> class ISL_t, template<class, template<class> class> class Data_t>
void BM_Delete(benchmark::State& st) {
  for (auto _ : st) {
    st.PauseTiming();
    Data_t<Interval_t, ISL_t> data(st.range());
    std::vector<Interval_t> intervals(data.isl.begin(), data.isl.end());
    std::shuffle(intervals.begin(), intervals.end(), std::mt19937(std::random_device()()));
    st.ResumeTiming();

    for (auto const& interval : intervals) {
        data.isl.remove(interval);
    }
  }
}

static void DeleteArguments(benchmark::internal::Benchmark* b) {
  for (int i = 10; i <= 1000; i *= 10) {
    b->Arg(i);
  }
  for (int i = 2; i <= 10; ++i) {
    b->Arg(1000 * i);
  }
}

BENCHMARK(BM_Delete<Interval_skip_list_interval<double>, Interval_skip_list, Sparse_data>)
    ->Name("DeleteSparseISL")
    ->Apply(DeleteArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Delete<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list, Sparse_data>)
    ->Name("DeleteSparseCGAL")
    ->Apply(DeleteArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Delete<Interval_skip_list_interval<double>, Interval_skip_list, Dense_data>)
    ->Name("DeleteDenseISL")
    ->Apply(DeleteArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Delete<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list, Dense_data>)
    ->Name("DeleteDenseCGAL")
    ->Apply(DeleteArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Delete<Interval_skip_list_interval<double>, Interval_skip_list, Random_data>)
    ->Name("DeleteRandomISL")
    ->Apply(DeleteArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Delete<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list, Random_data>)
    ->Name("DeleteRandomCGAL")
    ->Apply(DeleteArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

class Noop_iterator {
public:
  Noop_iterator() = default;
  template<class T>
  Noop_iterator& operator=(const T& ignored) { return *this; }
  Noop_iterator& operator++() { return *this; }
  Noop_iterator operator++(int) { return *this; }
  Noop_iterator& operator*() { return *this; }
};

template<class Interval_t, template<class> class ISL_t, template<class, template<class> class> class Data_t>
void BM_Search(benchmark::State& st) {
  Data_t<Interval_t, ISL_t> data(st.range());
  std::vector<typename Interval_t::Value> endpoints;
  endpoints.reserve(2 * st.range());
  for (auto it = data.isl.begin(); it != data.isl.end(); ++it) {
    endpoints.push_back(it->inf());
    endpoints.push_back(it->sup());
  }
  std::sort(endpoints.begin(), endpoints.end());
  endpoints.erase(std::unique(endpoints.begin(), endpoints.end()), endpoints.end());
  for (auto _ : st) {
    // std::vector<Interval_t> v;
    // v.reserve(st.range());
    for (auto const& q : endpoints) {
      Noop_iterator it;
      // data.isl.find_intervals(q, std::back_inserter(v));
      benchmark::DoNotOptimize(it);
      data.isl.find_intervals(q, it);
    }
  }
}

static void SearchArguments(benchmark::internal::Benchmark* b) {
  for (int i = 10; i <= 10000; i *= 10) {
    b->Arg(i);
  }
  for (int i = 2; i <= 10; ++i) {
    b->Arg(10000 * i);
  }
}

BENCHMARK(BM_Search<Interval_skip_list_interval<double>, Interval_skip_list, Sparse_data>)
    ->Name("SearchSparseISL")
    ->Apply(SearchArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Search<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list, Sparse_data>)
    ->Name("SearchSparseCGAL")
    ->Apply(SearchArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Search<Interval_skip_list_interval<double>, Interval_skip_list, Dense_data>)
    ->Name("SearchDenseISL")
    ->Apply(SearchArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Search<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list, Dense_data>)
    ->Name("SearchDenseCGAL")
    ->Apply(SearchArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Search<Interval_skip_list_interval<double>, Interval_skip_list, Random_data>)
    ->Name("SearchRandomISL")
    ->Apply(SearchArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Search<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list, Random_data>)
    ->Name("SearchRandomCGAL")
    ->Apply(SearchArguments)
    ->Iterations(10)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
