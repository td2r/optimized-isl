#ifndef OPTIMIZED_INTERVAL_SKIP_LIST_UTILS_H
#define OPTIMIZED_INTERVAL_SKIP_LIST_UTILS_H

#include <random>

template<class Interval_t, template<class> class ISL_t>
struct Sparse_data {
  ISL_t<Interval_t> isl;

  explicit Sparse_data(int size) {
    int sgn = 1;
    for (int i = 1; i <= size; ++i) {
      int inf = sgn * i;
      int sup = sgn * (i + 1);
      if (inf > sup)
        std::swap(inf, sup);
      isl.insert(Interval_t(inf, sup, true, true));
      sgn = -sgn;
    }
  }
};

template<class Interval_t, template<class> class ISL_t>
struct Dense_data {
  ISL_t<Interval_t> isl;

  explicit Dense_data(int size) {
    for (int i = 1; i <= size; ++i) {
      int x = size + 1 - i;
      isl.insert(Interval_t(-x, x, true, true));
    }
  }
};

template<class Interval_t, template<class> class ISL_t>
struct Random_data {
  int const COORD_RANGE = 10000;
  std::mt19937 gen;
  std::uniform_int_distribution<int> uniform;
  ISL_t<Interval_t> isl;

  explicit Random_data(int size) : gen(std::random_device()()), uniform(-COORD_RANGE, COORD_RANGE), isl() {
    for (int i = 0; i < size; ++i) {
      int inf = uniform(gen);
      int sup = uniform(gen);
      if (inf > sup)
        std::swap(inf, sup);
      bool inf_closed = inf == sup || gen() & 1; // avoid empty intervals
      bool sup_closed = inf == sup || gen() & 1; // avoid empty intervals
      isl.insert(Interval_t(inf, sup, inf_closed, sup_closed));
    }
  }
};

#endif //OPTIMIZED_INTERVAL_SKIP_LIST_UTILS_H
