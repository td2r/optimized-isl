#include <iostream>
#include <random>

//#define USE_CGAL_SKIP_LIST

#ifndef USE_CGAL_SKIP_LIST
  #include "include/Interval_skip_list.h"
  #include "include/Interval_skip_list_interval.h"
  typedef Interval_skip_list_interval<double> Interval_t;
  typedef Interval_skip_list<Interval_t> ISL;
#else
  #include <CGAL/Interval_skip_list.h>
  #include <CGAL/Interval_skip_list_interval.h>
  typedef CGAL::Interval_skip_list_interval<double> Interval_t;
  typedef CGAL::Interval_skip_list<Interval_t> ISL;
#endif

size_t const n = 1000000;

int main() {
  std::mt19937 gen((std::random_device()()));
  std::uniform_int_distribution<int> uniform(-100000, 100000);

  int* arr = new int[5];
  arr[8] = 8;
  std::cout << arr[0];
  return 0;

  ISL isl;
  for (auto i = 0; i < n; ++i) {
    int a = uniform(gen);
    int b = uniform(gen);
    if (a > b)
      std::swap(a, b);
    isl.insert(Interval_t(a, b, gen() & 1, gen() & 1));
  }

//  std::cout << "Enter your boob size:" << std::endl;
//  int ignored;
//  std::cin >> ignored;

  return 0;
}
