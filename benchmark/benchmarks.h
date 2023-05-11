#ifndef BENCHMARKS_H
#define BENCHMARKS_H

#include "../utils/utils.h"

#include <random>

#include <benchmark/benchmark.h>

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
  // st.SetComplexityN(st.range(0));
}

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

template<int N>
void DecimalArgs(benchmark::internal::Benchmark* b) {
  for (int i = 10; i * 10 < N; i *= 10) {
    b->Arg(i);
  }
  for (int i = 1; i <= 10; ++i) {
    b->Arg((N / 10) * i);
  }
}

#endif //BENCHMARKS_H
