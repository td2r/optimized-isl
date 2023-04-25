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
  std::cout << sizeof(IntervalSLnode<Interval_t>) << std::endl;
  std::cout << sizeof(Interval_t) << std::endl;

  return 0;
}
