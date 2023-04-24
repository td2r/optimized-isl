#include "../include/Interval_skip_list.h"
#include "../include/Interval_skip_list_interval.h"

#include <gtest/gtest.h>
#include <CGAL/Interval_skip_list.h>
#include <CGAL/Interval_skip_list_interval.h>

#include <random>

typedef Interval_skip_list_interval<double> Interval_t;
typedef Interval_skip_list<Interval_t> ISL_t;

auto isl_seed = std::random_device()();
auto seed = std::random_device()();

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
  Self_ operator++(int) { return *this; }
  Self_& operator*() { return *this; }
};

template<class Interval>
size_t count_stabs(typename Interval::Value const& q, Interval_skip_list<Interval> const& isl) {
  size_t cnt = 0;
  isl.find_intervals(q, count_iterator<size_t>(cnt));
  return cnt;
}

class ISLTest : public ::testing::Test {
protected:
  void SetUp() override {
    gen.seed(seed);
    isl.seed(isl_seed);
  }

  // void TearDown() override {}

  ISL_t isl;
  std::mt19937 gen;
};

TEST_F(ISLTest, BasicFunctionality) {
  const size_t n = 10;
  const double d = 3.0;

  std::vector<Interval_t> intervals;
  for (int i = 0; i < n; ++i) {
    intervals.emplace_back(i, i + d);
  }

  std::shuffle(intervals.begin(), intervals.end(), gen);
  for (auto const& i : intervals) {
    isl.insert(i);
  }

  for (int i = 0; i < n + d; ++i) {
    EXPECT_EQ(
        std::count_if(intervals.begin(), intervals.end(), [&i](auto const& interval) {
          return interval.contains(i);
        }),
        count_stabs(i, isl)
    );
  }
}

TEST_F(ISLTest, InsertRemoveTest) {
  const size_t n = 5;

  for (int i = 0; i < 2 * n; i += 2) {
    isl.insert(Interval_t(i, i + 1));
  }
  for (int i = 2 * n; i > 0; i -= 2) {
    isl.insert(Interval_t(i - 1, i));
  }
  for (int i = 1; i < 2 * n; ++i) {
    EXPECT_EQ(2, count_stabs(i, isl));
  }
  for (int i = 2 * n; i > 0; i -= 2) {
    isl.remove(Interval_t(i - 1, i));
  }
  EXPECT_EQ(n, isl.size());
  for (int i = 0; i < 2 * n; ++i) {
    EXPECT_EQ(1, count_stabs(i, isl));
  }
  EXPECT_EQ(0, count_stabs(2 * n, isl));
  for (int i = 0; i < 2 * n; i += 2) {
    isl.remove(Interval_t(i, i + 1));
  }
  EXPECT_EQ(0, isl.size());
}

TEST_F(ISLTest, SemiopenIntervals) {
  std::list<Interval_t> list;
  Interval_t i1;
  Interval_t i2;

  i1 = Interval_t(-5, 0, true, false);
  i2 = Interval_t(0, 5, true, false);
  isl.insert(i1);
  isl.insert(i2);
  isl.find_intervals(0, std::front_inserter(list));
  EXPECT_EQ(1, list.size());
  EXPECT_EQ(i2, *list.begin());
  list.clear();

  isl.remove(i2);
  i2 = Interval_t(0, 5, false, true);
  isl.insert(i2);
  EXPECT_EQ(0, count_stabs(0, isl));

  isl.remove(i1);
  i1 = Interval_t(-5, 0, false, true);
  isl.insert(i1);
  isl.find_intervals(0, std::front_inserter(list));
  EXPECT_EQ(1, list.size());
  EXPECT_EQ(i1, *list.begin());
  list.clear();

  isl.remove(i2);
  i2 = Interval_t(0, 5, true, false);
  isl.insert(i2);
  EXPECT_EQ(2, count_stabs(0, isl));

  isl.clear();
  isl.insert(Interval_t(-5, 0, false, false));
  isl.insert(Interval_t(0, 5, false, false));
  EXPECT_EQ(0, count_stabs(0, isl));
  EXPECT_EQ(0, count_stabs(-5, isl));
  EXPECT_EQ(0, count_stabs(5, isl));
  EXPECT_EQ(1, count_stabs(-2, isl));
  EXPECT_EQ(1, count_stabs(3, isl));
}

TEST_F(ISLTest, MultipleSameIntervals) {
  size_t const n = 10;
  Interval_t interval(0, n);
  for (int i = 0; i < n; ++i) {
    isl.insert(interval);
  }
  for (int i = 0; i < n; ++i) {
    EXPECT_EQ(n - i, count_stabs(0, isl));
    isl.remove(interval);
  }
  EXPECT_EQ(0, isl.size());
}

TEST_F(ISLTest, EpsilonTest) {
  Interval_t interval(1,
                      static_cast<Interval_t::Value>(1) + std::numeric_limits<Interval_t::Value>::epsilon(),
                      false,
                      true);
  isl.insert(interval);
  EXPECT_EQ(1, isl.size());
  EXPECT_EQ(interval, *isl.begin());
  EXPECT_EQ(0, count_stabs(interval.inf(), isl));
  EXPECT_EQ(1, count_stabs(interval.sup(), isl));
  EXPECT_TRUE(isl.remove(interval));
}

TEST_F(ISLTest, EmptyIntervals) {
  Interval_t i1(2, 2, false, false);
  Interval_t i2(2, 2, true, false);
  isl.insert(i1);
  isl.insert(i1);
  EXPECT_EQ(2, isl.size());
  EXPECT_EQ(0, count_stabs(2, isl));
  isl.remove(i1);
  EXPECT_EQ(1, isl.size());
  isl.insert(i2);
  EXPECT_EQ(2, isl.size());
  EXPECT_EQ(0, count_stabs(2, isl));
  isl.remove(i1);
  EXPECT_EQ(1, isl.size());
  EXPECT_EQ(i2, *isl.begin());
  EXPECT_TRUE(isl.remove(i2));
}

// TODO: random test

TEST_F(ISLTest, ManyIntervals) {
  int const n = 100000;
  std::uniform_int_distribution<int> dist(1, n - 1);
  isl.insert(Interval_t(0, 0));
  for (int i = 0; i < n; ++i) {
    int a = dist(gen);
    int b = dist(gen);
    if (gen() % 2) {
      a = -a;
      b = -b;
    }
    isl.insert(Interval_t(std::min(a, b), std::max(a, b), gen() % 2, gen() % 2));
  }
  isl.insert(Interval_t(n, n));
  EXPECT_EQ(n + 2, isl.size());
  EXPECT_EQ(0, count_stabs(-n, isl));
  EXPECT_EQ(1, count_stabs(0, isl));
  EXPECT_EQ(1, count_stabs(n, isl));
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  std::cout << "isl_seed: " << isl_seed << std::endl <<
               "seed:     " << seed << std::endl;
  return RUN_ALL_TESTS();
}
