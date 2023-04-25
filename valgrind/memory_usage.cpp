#include "../utils/utils.h"

#include "../include/Interval_skip_list.h"
#include "../include/Interval_skip_list_interval.h"

#include <CGAL/Interval_skip_list.h>
#include <CGAL/Interval_skip_list_interval.h>

#include <cstdlib>
#include <chrono>
#include <thread>

const char OPTIMIZED_ISL_NAME[] = "Optimized";
const char CGAL_ISL_NAME[] = "CGAL";

const char SPARSE_DATA_NAME[] = "Sparse";
const char DENSE_DATA_NAME[] = "Dense";
const char RANDOM_DATA_NAME[] = "Random";

template<class Interval_t, template<class> class ISL_t, template<class, template<class> class> class Data_t>
static void pump_memory(char** sz) {
  int size = atoi(*sz);
  if (!size) {
    std::cerr << "incorrect size: " << *sz << std::endl;
    throw std::runtime_error("");
  }
  Data_t<Interval_t, ISL_t> data(size);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

template<class Interval_t, template<class> class ISL_t>
void choose_data_type(char** name) {
  if (strcmp(*name, SPARSE_DATA_NAME) == 0) {
    pump_memory<Interval_t, ISL_t, Sparse_data>(name + 1);
  } else if (strcmp(*name, DENSE_DATA_NAME) == 0) {
    pump_memory<Interval_t, ISL_t, Dense_data>(name + 1);
  } else if (strcmp(*name, RANDOM_DATA_NAME) == 0) {
    pump_memory<Interval_t, ISL_t, Random_data>(name + 1);
  } else {
    std::cerr << "unknown data type: " << *name << std::endl;
    throw std::runtime_error("");
  }
}

static void choose_data_structure(char** name) {
  if (strcmp(*name, OPTIMIZED_ISL_NAME) == 0) {
    choose_data_type<Interval_skip_list_interval<double>, Interval_skip_list>(name + 1);
  } else if (strcmp(*name, CGAL_ISL_NAME) == 0) {
    choose_data_type<CGAL::Interval_skip_list_interval<double>, CGAL::Interval_skip_list>(name + 1);
  } else {
    std::cerr << "unknown data structure: " << *name << std::endl;
    throw std::runtime_error("");
  }
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: ./memory_usage DATA_STRUCTURE DATA_TYPE SIZE" << std::endl;
    return 1;
  }
  choose_data_structure(argv + 1);
  return 0;
}
