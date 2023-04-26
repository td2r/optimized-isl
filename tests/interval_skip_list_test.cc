#include "../utils/utils.h"
#include "../include/Interval_skip_list.h"
#include "../include/Interval_skip_list_interval.h"

#include <CGAL/Interval_skip_list.h>
#include <CGAL/Interval_skip_list_interval.h>
#include <gtest/gtest.h>

#include <random>

auto isl_seed = std::random_device()();
auto seed = std::random_device()();

typedef Interval_skip_list_interval<double> Interval_t;
typedef Interval_skip_list<Interval_t> ISL_t;

size_t count_stabs(typename Interval_t::Value const& q, ISL_t& isl) { // CGAL find_intervals not marked as const
  size_t cnt = 0;
  isl.find_intervals(q, count_iterator<size_t>(cnt));
  return cnt;
}

class ISLTest : public ::testing::Test {
protected:
  ISL_t isl;
  std::mt19937 gen;

  void SetUp() override {
    gen.seed(seed);
    isl.seed(isl_seed);
  }

  // void TearDown() override {}

  void expect_find_intervals(typename Interval_t::Value const& q,
                             std::vector<Interval_t> const& intervals)
  {
    std::vector<Interval_t> found;
    std::copy_if(intervals.begin(), intervals.end(), std::back_inserter(found), [&q](Interval_t const& interval) {
      return interval.contains(q);
    });
    std::vector<Interval_t> from_isl;
    isl.find_intervals(q, std::back_inserter(from_isl));
    std::sort(found.begin(), found.end(), interval_tuple_comparator<Interval_t>());
    std::sort(from_isl.begin(), from_isl.end(), interval_tuple_comparator<Interval_t>());
    EXPECT_EQ(found, from_isl);
  }

  template<int N>
  void RandomTest();
};

extern "C" {
void __ubsan_on_report() {
  FAIL() << "Encountered an undefined behavior sanitizer error";
}
void __asan_on_error() {
  FAIL() << "Encountered an address sanitizer error";
}
void __tsan_on_report() {
  FAIL() << "Encountered a thread sanitizer error";
}
}  // extern "C"

TEST_F(ISLTest, Empty) {
  ISL_t new_isl;

  EXPECT_EQ(0, new_isl.size());
  EXPECT_EQ(new_isl.begin(), new_isl.end());
}

TEST_F(ISLTest, RangeConstructor) {
  std::vector<Interval_t> intervals({
    Interval_t(-1, 1, true, true),
    Interval_t(-1, 0, true, true),
    Interval_t(0, 1, true, true)
  });
  for (auto const& i : intervals) {
    isl.insert(i);
  }
  ISL_t other(intervals.begin(), intervals.end());
  EXPECT_EQ(isl.size(), other.size());
  auto it1 = isl.begin();
  auto it2 = other.begin();
  for (size_t i = 0; i < isl.size(); ++i) {
    EXPECT_EQ(*it1, *it2);
    ++it1;
    ++it2;
  }
}

TEST_F(ISLTest, Insert) {
  Interval_t i(-2, 3, true, false);
  isl.insert(i);
  EXPECT_EQ(1, isl.size());
  EXPECT_EQ(i, *isl.begin());
}

TEST_F(ISLTest, InsertMultiple) {
  std::vector<Interval_t> intervals({
    Interval_t(-5, 4, true, true),
    Interval_t(-3, 5, false, true),
    Interval_t(0, 0, true, true)
  });
  for (auto const& interval : intervals) {
    isl.insert(interval);
  }
  std::vector<Interval_t> from_isl(isl.begin(), isl.end());
  std::sort(intervals.begin(), intervals.end(), interval_tuple_comparator<Interval_t>());
  std::sort(from_isl.begin(), from_isl.end(), interval_tuple_comparator<Interval_t>());
  EXPECT_EQ(intervals, from_isl);
}

TEST_F(ISLTest, InsertDuplicates) {
  Interval_t interval(-10, 0, false, true);
  int const copies = 3;
  for (int i = 0; i < copies; ++i) {
    isl.insert(interval);
  }
  EXPECT_EQ(copies, isl.size());
  auto it = isl.begin();
  for (int i = 0; i < copies; ++i) {
    EXPECT_EQ(interval, *it);
    ++it;
  }
}

TEST_F(ISLTest, InsertSimilar) {
  double const inf = 5.0;
  double const sup = 7.0;
  std::vector<Interval_t> intervals({
      Interval_t(inf, sup, true, true),
      Interval_t(inf, sup, false, true)
  });
  isl.insert(intervals[0]);
  isl.insert(intervals[1]);
  EXPECT_EQ(2, isl.size());
  std::vector<Interval_t> from_isl(isl.begin(), isl.end());
  std::sort(intervals.begin(), intervals.end(), interval_tuple_comparator<Interval_t>());
  std::sort(from_isl.begin(), from_isl.end(), interval_tuple_comparator<Interval_t>());
  EXPECT_EQ(intervals, from_isl);
}

TEST_F(ISLTest, RemoveFromEmpty) {
  EXPECT_FALSE(isl.remove(Interval_t(-2, 1, true, true)));
  EXPECT_EQ(0, isl.size());
}

TEST_F(ISLTest, Remove) {
  Interval_t interval(10, 11, false, true);
  isl.insert(interval);
  EXPECT_TRUE(isl.remove(interval));
  EXPECT_EQ(0, isl.size());
}

TEST_F(ISLTest, RemoveNonExistent) {
  Interval_t interval(-2, 2, true, true);
  Interval_t nonexistent(-1, 1, true, true);
  isl.insert(interval);
  EXPECT_FALSE(isl.remove(nonexistent));
  EXPECT_EQ(1, isl.size());
  EXPECT_EQ(interval, *isl.begin());
}

TEST_F(ISLTest, RemoveDuplicate) {
  Interval_t interval(-3, 3, false, true);
  int const copies = 3;
  for (int i = 0; i < copies; ++i) {
    isl.insert(interval);
  }
  EXPECT_TRUE(isl.remove(interval));
  EXPECT_EQ(copies - 1, isl.size());
  auto it = isl.begin();
  for (int i = 0; i < copies - 1; ++i) {
    EXPECT_EQ(*it, interval);
    ++it;
  }
}

TEST_F(ISLTest, RemoveSimilar) {
  double const inf = 5.0;
  double const sup = 7.0;
  std::vector<Interval_t> intervals({
      Interval_t(inf, sup, true, true),
      Interval_t(inf, sup, false, true)
  });
  isl.insert(intervals[0]);
  isl.insert(intervals[1]);
  EXPECT_TRUE(isl.remove(intervals[0]));
  EXPECT_EQ(1, isl.size());
  EXPECT_EQ(intervals[1], *isl.begin());
}

TEST_F(ISLTest, ClearEmpty) {
  isl.clear();
  EXPECT_EQ(0, isl.size());
  EXPECT_EQ(isl.begin(), isl.end());
}

TEST_F(ISLTest, Clear) {
  for (int i = 0; i < 5; ++i) {
    isl.insert(Interval_t(i - 8, i + 3, true, false));
  }
  EXPECT_EQ(5, isl.size());
  isl.clear();
  EXPECT_EQ(0, isl.size());
  EXPECT_EQ(isl.begin(), isl.end());
}

TEST_F(ISLTest, IsContainedEmpty) {
  EXPECT_FALSE(isl.is_contained(42));
}

TEST_F(ISLTest, IsContainedSingle) {
  double const inf = -1;
  double const sup = 5;
  isl.insert(Interval_t(inf, sup, true, true));
  EXPECT_FALSE(isl.is_contained(inf - 1));
  EXPECT_TRUE(isl.is_contained(inf));
  EXPECT_TRUE(isl.is_contained((inf + sup) / 2.0));
  EXPECT_TRUE(isl.is_contained(sup));
  EXPECT_FALSE(isl.is_contained(sup + 1));
}

TEST_F(ISLTest, IsContainedSemiopen) {
  double const x[] = {-1, 3, 5};
  isl.insert(Interval_t(x[0], x[1], true, false));
  isl.insert(Interval_t(x[1], x[2], false, true));
  EXPECT_TRUE(isl.is_contained(x[0]));
  EXPECT_TRUE(isl.is_contained((x[0] + x[1]) / 2.0));
  EXPECT_FALSE(isl.is_contained(x[1]));
  EXPECT_TRUE(isl.is_contained((x[1] + x[2]) / 2.0));
  EXPECT_TRUE(isl.is_contained(x[2]));
}

TEST_F(ISLTest, IsContainedSemiopenOverlaping) {
  double const inf = -7;
  double const sup = -1;
  isl.insert(Interval_t(inf, sup, false, true));
  isl.insert(Interval_t(inf, sup, true, false));
  for (double x : {inf, (inf + sup) / 2.0, sup}) {
    EXPECT_TRUE(isl.is_contained(x));
  }
  EXPECT_FALSE(isl.is_contained(inf - 1));
  EXPECT_FALSE(isl.is_contained(sup + 1));
}

TEST_F(ISLTest, IsContainedMultiple) {
  double x[] = {-11, -5, -1, 3};
  isl.insert(Interval_t(x[0], x[1]));
  isl.insert(Interval_t(x[2], x[3]));
  EXPECT_FALSE(isl.is_contained(x[0] - 10));
  EXPECT_TRUE(isl.is_contained((x[0] + x[1]) / 2.0));
  EXPECT_FALSE(isl.is_contained((x[1] + x[2]) / 2.0));
  EXPECT_TRUE(isl.is_contained((x[2] + x[3]) / 2.0));
  EXPECT_FALSE(isl.is_contained(x[3] + 10));
}

TEST_F(ISLTest, IsContainedOverlaping) {
  double x[] = {-5, 0, 3};
  isl.insert(Interval_t(x[0], x[1], true, true));
  isl.insert(Interval_t(x[0], x[2], true, true));
  EXPECT_TRUE(isl.is_contained((x[0] + x[1]) / 2.0));
  EXPECT_TRUE(isl.is_contained((x[1] + x[2]) / 2.0));
  EXPECT_FALSE(isl.is_contained(x[2] + 100));
}

TEST_F(ISLTest, FindIntervals) {
  int const n = 10;
  double const d = 3.0;
  std::vector<Interval_t> intervals(n);
  for (int i = 0; i < n; ++i) {
    intervals[i] = Interval_t(i, i + d);
    isl.insert(intervals[i]);
  }
  for (int i = -1; i <= n + d + 1; ++i) {
    expect_find_intervals(i, intervals);
  }
}

TEST_F(ISLTest, FindIntervalsRandomInsertOrder) {
  const int n = 100;
  const double d = 30.0;

  std::vector<Interval_t> intervals;
  for (int i = 0; i < n; ++i) {
    intervals.emplace_back(i, i + d);
  }

  std::shuffle(intervals.begin(), intervals.end(), gen);
  for (auto const& i : intervals) {
    isl.insert(i);
  }

  for (int i = 0; i < n + d; ++i) {
    expect_find_intervals(i, intervals);
  }
}

TEST_F(ISLTest, FindIntervalsSemiopen) {
  double const inf = -5;
  double const sup = 8;
  Interval_t intervals[] = {
      Interval_t(inf, sup, false, true),
      Interval_t(inf, sup, true, false)
  };
  isl.insert(intervals[0]);
  isl.insert(intervals[1]);
  std::vector<Interval_t> found;
  isl.find_intervals(inf, std::back_inserter(found));
  EXPECT_EQ(1, found.size());
  EXPECT_EQ(intervals[1], found[0]);
  found.clear();
  isl.find_intervals((inf + sup) / 2.0, std::back_inserter(found));
  EXPECT_EQ(2, found.size());
  found.clear();
  isl.find_intervals(sup, std::back_inserter(found));
  EXPECT_EQ(1, found.size());
  EXPECT_EQ(intervals[0], found[0]);
}

TEST_F(ISLTest, FindIntervalsDuplicates) {
  double x[] = {0, 5, 12};
  std::vector<Interval_t> intervals({
      Interval_t(x[0], x[1]),
      Interval_t(x[1], x[2], false, true),
      Interval_t(x[0], x[1])
  });
  for (auto const& interval : intervals) {
    isl.insert(interval);
  }
  std::vector<Interval_t> found;
  isl.find_intervals(x[1], std::back_inserter(found));
  EXPECT_EQ(2, found.size());
  EXPECT_EQ(intervals[0], found[0]);
  EXPECT_EQ(intervals[0], found[1]);
}

TEST_F(ISLTest, InsertRemoveOverlaped) {
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

TEST_F(ISLTest, SemiopenIntervalsInsertRemove) {
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

TEST_F(ISLTest, DuplicatesRemoveFind) {
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

TEST_F(ISLTest, Epsilon) {
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

template<int N>
void ISLTest::RandomTest() {
  int const n = N;
  std::uniform_int_distribution<int> uniform(-n, n);
  std::vector<Interval_t> intervals(n);
  std::vector<double> ep(2 * n);
  for (int i = 0; i < n; ++i) {
    int inf = uniform(gen);
    int sup = uniform(gen);
    if (inf > sup)
      std::swap(inf, sup);
    intervals[i] = Interval_t(inf, sup, gen() & 1, gen() & 1);
    ep[2 * i] = inf;
    ep[2 * i + 1] = sup;
    isl.insert(intervals[i]);
  }
  std::sort(ep.begin(), ep.end());
  ep.erase(std::unique(ep.begin(), ep.end()), ep.end());
  for (double const & q : ep) {
    expect_find_intervals(q, intervals);
  }
}

TEST_F(ISLTest, Random10) {
  RandomTest<10>();
}

TEST_F(ISLTest, Random100) {
  RandomTest<100>();
}

TEST_F(ISLTest, Random1000) {
  RandomTest<1000>();
}

TEST_F(ISLTest, Random3000) {
  RandomTest<3000>();
}

TEST_F(ISLTest, DeathTest) {
  auto create_big_isl = [&](){
    int const n = 1000000;
    std::uniform_int_distribution<int> dist(-n, n);
    isl.insert(Interval_t(0, 0));
    for (int i = 0; i < n; ++i) {
      int inf = dist(gen);
      int sup = dist(gen);
      if (inf > sup)
        std::swap(inf, sup);
      isl.insert(Interval_t(inf, sup, gen() % 2, gen() % 2));
    }
    EXPECT_LE(1, count_stabs(0, isl));
    exit(0);
  };
  EXPECT_EXIT(create_big_isl(), testing::ExitedWithCode(0), "");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  std::cout << "isl_seed: " << isl_seed << std::endl <<
               "seed:     " << seed << std::endl;
  return RUN_ALL_TESTS();
}
