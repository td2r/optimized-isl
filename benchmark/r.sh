#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd "$SCRIPT_DIR"

cmake --build ../cmake-build-release --target isl_benchmark -j 6

sudo cpupower frequency-set --governor performance
taskset -c 0 ../cmake-build-release/isl_benchmark --benchmark_format=csv --benchmark_filter=Insert > ./insert.csv
taskset -c 0 ../cmake-build-release/isl_benchmark --benchmark_format=csv --benchmark_filter=Delete > ./delete.csv
taskset -c 0 ../cmake-build-release/isl_benchmark --benchmark_format=csv --benchmark_filter=Search > ./search.csv
sudo cpupower frequency-set --governor powersave

# python3 plot.py -f insert.csv
