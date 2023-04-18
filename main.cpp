#include <iostream>

#include "include/Interval_skip_list.h"
#include "include/Interval_skip_list_interval.h"
//#include <CGAL/Interval_skip_list.h>
//#include <CGAL/Interval_skip_list_interval.h>
//#include <CGAL/algorithm.h>

typedef Interval_skip_list_interval<double> MyInterval;
typedef Interval_skip_list<MyInterval> My_Interval_skip_list;

int main() {
  MyInterval i1 = MyInterval(1, 2, false, false);
  MyInterval i2 = MyInterval(3, 4);
  My_Interval_skip_list isl;
  isl.insert(i1);
  isl.insert(i2);
//  std::cout << std::boolalpha << isl.is_contained(1) << std::endl;
//  std::cout << std::boolalpha << isl.is_contained(1.5) << std::endl;
//  std::cout << std::boolalpha << isl.is_contained(2) << std::endl;
//  std::cout << std::boolalpha << isl.is_contained(2.5) << std::endl;
//  std::cout << std::boolalpha << isl.is_contained(3.0) << std::endl;
//  std::cout << std::boolalpha << isl.is_contained(3.5) << std::endl;
//  std::cout << std::boolalpha << isl.is_contained(4.0) << std::endl;
//  std::cout << std::boolalpha << isl.is_contained(4.5) << std::endl;
  std::cout << isl.size() << std::endl;
  isl.remove(MyInterval(0, 0));
  std::cout << isl.size() << std::endl;
  isl.remove(i2);
  std::cout << isl.size() << std::endl;
  std::cout << std::boolalpha << isl.is_contained(3.5) << std::endl <<
                                 isl.is_contained(1.5) << std::endl;
  isl.remove(i2);
  std::cout << isl.size() << std::endl;
  isl.remove(i1);
  std::cout << isl.size() << std::endl;
  isl.remove(i1);
  std::cout << isl.size() << std::endl;
  return 0;
}
