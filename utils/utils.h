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
  // int const COORD_RANGE = 1000000000;
  std::mt19937 gen;
  std::uniform_int_distribution<int> uniform;
  ISL_t<Interval_t> isl;

  explicit Random_data(int size) : gen(std::random_device()()), uniform(-size, size), isl() {
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

template<class IntType = int>
class count_iterator {
private:
  typedef count_iterator<IntType> Self_;

  IntType* counter_ptr;
public:
  explicit count_iterator(IntType& counter) : counter_ptr(&counter) {}
  template<class T>
  Self_& operator=(const T& value) {
    return *this;
  }
  Self_& operator++() {
    ++(*counter_ptr);
    return *this;
  }
  Self_ operator++(int) {
    ++(*counter_ptr);
    return *this;
  }
  Self_& operator*() { return *this; }
};

// order interval just as tuple
template<class Interval_t>
struct interval_tuple_comparator {
  bool operator()(Interval_t const& i1, Interval_t const& i2) const {
    if (i1.inf() != i2.inf()) {
      return i1.inf() < i2.inf();
    } else if (i1.sup() != i2.sup()) {
      return i1.sup() < i2.sup();
    } else if (i1.inf_closed() != i2.inf_closed()) {
      return i1.inf_closed();
    } else if (i1.sup_closed() != i2.sup_closed()) {
      return i1.sup_closed();
    }
    return false;
  }
};

class Noop_iterator {
  public:
  Noop_iterator() = default;
  template<class T>
  Noop_iterator& operator=(const T& ignored) { return *this; }
  Noop_iterator& operator++() { return *this; }
  Noop_iterator operator++(int) { return *this; }
  Noop_iterator& operator*() { return *this; }
};

#endif //OPTIMIZED_INTERVAL_SKIP_LIST_UTILS_H
