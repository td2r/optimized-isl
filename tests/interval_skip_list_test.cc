#include "../include/Interval_skip_list.h"
#include "../include/Interval_skip_list_interval.h"

#include <gtest/gtest.h>

#include <random>

typedef Interval_skip_list_interval<double> Interval_double;

auto rng_engine = std::mt19937(std::random_device()());

template<class IntType = int>
class count_iterator {
private:
  typedef count_iterator<IntType> Self_;

  IntType* counter_ptr;
public:
  explicit count_iterator(IntType& counter) : counter_ptr(&counter) {}
  template<class T>
  Self_& operator=(const T& value) {
    ++(*counter_ptr);
    return *this;
  }
  Self_& operator++() { return *this; }
  Self_& operator++(int) { return *this; }
  Self_& operator*() { return *this; }
};

template<class Interval>
size_t count_stabs(Interval_skip_list<Interval> const& isl, typename Interval::Value q) {
  size_t cnt = 0;
  isl.find_intervals(q, count_iterator<size_t>(cnt));
  return cnt;
}

// Demonstrate some basic assertions.
TEST(SimpleTest, ISLTests) {
  Interval_skip_list<Interval_double> isl;
  std::vector<Interval_double> intervals;
  const size_t n = 10000;
  const size_t len = 200000;
  for (int i = 0; i < n; ++i) {
    intervals.emplace_back(i, i + len);
  }
  std::shuffle(intervals.begin(), intervals.end(), rng_engine);
  for (auto const& i : intervals) {
    isl.insert(i);
//    std::cout << isl << "=============================================" << std::endl;
  }
//  std::cout << isl << "=============================================" << std::endl;
  for (int i = 0; i < n; ++i) {
    EXPECT_EQ(i + 1, count_stabs(isl, i));
  }
  for (int i = n; i <= len; ++i) {
    EXPECT_EQ(n, count_stabs(isl, i));
  }
  for (int i = len; i < n + len; ++i) {
    EXPECT_EQ(n + len - i, count_stabs(isl, i));
  }
}
