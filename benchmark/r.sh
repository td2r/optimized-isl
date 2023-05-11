#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT="$(realpath "$SCRIPT_DIR/..")"
BIN="$(realpath "$ROOT/cmake-build-release")"

cd "$SCRIPT_DIR"

function bench_csv() {
    if [[ $# -lt 2 ]]; then
        echo "bench_csv called with $# arguments"
        echo "Usage: bench_csv CMAKE_TARGET BENCHMARK_REGEX [CSV_NAME]"
        exit 1
    fi
    local cmake_target="$1"
    local bench_regex="$2"
    local csv_name="$2"
    if [[ $# -gt 2 ]]; then
        csv_name="$3"
    fi
    taskset -c "$(shuf -i 0-7 -n 1)" \
        "$BIN/$cmake_target" \
        --benchmark_format=csv \
        --benchmark_filter="$bench_regex" \
        > "./csv/$csv_name.csv"
}

function run_comparison_benchmarks() {
    if [[ $# -lt 1 ]]; then
        echo "run_comparison_benchmarks called with $# arguments"
        echo "Usage: run_comparison_benchmarks STRUCT_TO_COMPARE_NAME"
        exit 1
    fi
    local ds="$1"
    for action in Insert Delete Search
    do
        for dtype in Sparse Dense Random
        do
            bench_csv "isl_${ds}_bench" "$action$dtype" "isl_${ds}_${action}_${dtype}"
        done
    done
}

function run_benchmarks() {
    mkdir -p ./csv
    sudo cpupower frequency-set --governor performance

    # bench_csv ... "Insert.*(ISL|CGAL)" InsertISL_CGAL

    # run_comparison_benchmarks cgal

    bench_csv isl_self_bench Insert SelfInsert
    bench_csv isl_self_bench Delete SelfDelete
    bench_csv isl_self_bench Search SelfSearch

    run_comparison_benchmarks cartesian

    sudo cpupower frequency-set --governor powersave
}

function draw_graphics() {
    # rm -rf ./graphics
    mkdir -p ./graphics
    for fn in ./csv/*.csv; do
        base=$(basename -- "$fn")
        name="${base%.*}"
        python3 plot.py -f "${fn}" --output "./graphics/${name}.png"
    done
}

# build release
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_MAKE_PROGRAM=ninja \
      -DCMAKE_CXX_COMPILER=g++ \
      -G Ninja \
      -S "$ROOT" \
      -B "$BIN"
cmake --build "$BIN" --target isl_cgal_bench isl_self_bench isl_cartesian_bench -j 6

run_benchmarks
draw_graphics

