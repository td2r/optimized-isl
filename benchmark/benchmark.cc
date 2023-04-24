#include "../include/Interval_skip_list.h"
#include "../include/Interval_skip_list_interval.h"

#include <CGAL/Interval_skip_list.h>
#include <CGAL/Interval_skip_list_interval.h>

#include <benchmark/benchmark.h>

#include <random>

int const COORD_RANGE = 100000;
unsigned int seed = std::random_device()();
std::mt19937 gen;
std::uniform_int_distribution<int> uniform;

template<class Interval>
Interval randomInterval() {
    int l = uniform(gen);
    int r = uniform(gen);
    if (l > r)
        std::swap(l, r);
    bool inf_closed = l == r || gen() & 1;
    bool sup_closed = l == r || gen() & 1;
    return Interval(l, r, inf_closed, sup_closed);
}

template<class Interval_t, template<class> class ISL_t>
void BM_Insert(benchmark::State& st) {
  gen.seed(seed);
  uniform = std::uniform_int_distribution<int>(-COORD_RANGE, COORD_RANGE);
  ISL_t<Interval_t> isl;
  for (auto i = 0; i < st.range(); ++i) {
      isl.insert(randomInterval<Interval_t>());
  }
  Interval_t interval = randomInterval<Interval_t>();
  for (auto _ : st) {
    isl.insert(interval);

    st.PauseTiming();
    isl.remove(interval);
    interval = randomInterval<Interval_t>();
    st.ResumeTiming();
  }
}

static void BenchmarkArguments(benchmark::internal::Benchmark* b) {
  for (int i = 10; i < 1000; i *= 10) {
    b->Arg(i);
  }
  for (int i = 1; i <= 10; ++i) {
    b->Arg(i * 1000);
  }
}

BENCHMARK(BM_Insert<Interval_skip_list_interval<double>, Interval_skip_list>)
                ->Name("InsertOptimizedISL")
                ->Apply(BenchmarkArguments);

BENCHMARK(BM_Insert<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list>)
                ->Name("InsertCGAL")
                ->Apply(BenchmarkArguments);

template<class Interval_t, template<class> class ISL_t>
void BM_Delete(benchmark::State& st) {
  gen.seed(seed);
  uniform = std::uniform_int_distribution<int>(0, st.range() - 1);
  ISL_t<Interval_t> isl;
  std::vector<Interval_t> intervals(st.range());
  for (auto i = 0; i < st.range(); ++i) {
      intervals[i] = randomInterval<Interval_t>();
      isl.insert(intervals[i]);
  }
  int i = uniform(gen);
  for (auto _ : st) {
    isl.remove(intervals[i]);

    st.PauseTiming();
    isl.insert(intervals[i]);
    i = uniform(gen);
    st.ResumeTiming();
  }
}

BENCHMARK(BM_Delete<Interval_skip_list_interval<double>, Interval_skip_list>)
                ->Name("DeleteOptimizedISL")
                ->Apply(BenchmarkArguments);

BENCHMARK(BM_Delete<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list>)
                ->Name("DeleteCGAL")
                ->Apply(BenchmarkArguments);

class Noop_iterator {
private:
  int cnt = 0;
public:
  Noop_iterator() = default;
  template<class T>
  Noop_iterator& operator=(const T& ignored) { return *this; }
  Noop_iterator& operator++() { ++cnt; return *this; }
  Noop_iterator operator++(int) { ++cnt; return *this; }
  Noop_iterator& operator*() { return *this; }
};

template<class Interval_t, template<class> class ISL_t>
void BM_Search(benchmark::State& st) {
  gen.seed(seed);
  uniform = std::uniform_int_distribution<int>(-COORD_RANGE, COORD_RANGE);
  ISL_t<Interval_t> isl;
  int n = static_cast<int>(st.range());
  for (auto i = 0; i < st.range(); ++i) {
      isl.insert(randomInterval<Interval_t>());
      // isl.insert(Interval_t(-COORD_RANGE + i, COORD_RANGE - n + 1 + i, true, true));
  }
  int q = uniform(gen);
  for (auto _ : st) {
    // Noop_iterator it;
    // benchmark::DoNotOptimize(it);
    std::vector<Interval_t> v;
    v.reserve(n);
    auto data = v.data();
    benchmark::DoNotOptimize(data);
    isl.find_intervals(q, std::back_inserter(v));
    benchmark::ClobberMemory();

    st.PauseTiming();
    q = uniform(gen);
    st.ResumeTiming();
  }
}

static void SearchArguments(benchmark::internal::Benchmark* b) {
  for (int i = 10; i < 1000; i *= 10) {
    b->Arg(i);
  }
  for (int i = 1; i <= 10; ++i) {
    b->Arg(i * 1000);
  }
  b->Arg(30000)->Arg(70000)->Arg(100000);
}

BENCHMARK(BM_Search<Interval_skip_list_interval<double>, Interval_skip_list>)
                ->Name("SearchOptimizedISL")
                ->Apply(SearchArguments);

BENCHMARK(BM_Search<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list>)
                ->Name("SearchCGAL")
                ->Apply(SearchArguments);


BENCHMARK_MAIN();
