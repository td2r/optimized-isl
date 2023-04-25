#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd "$SCRIPT_DIR"

function bench_csv() {
    if [[ $# -eq 0 ]]; then
        exit 1
    fi
    local bench_regex="$1"
    local csv_name="$1"
    if [[ $# > 1 ]]; then
        csv_name="$2"
    fi
    taskset -c $(shuf -i 0-7 -n 1) \
        ../cmake-build-release/isl_benchmark \
        --benchmark_format=csv \
        --benchmark_filter="$bench_regex" \
        > ../csv/"$csv_name".csv
}

cmake --build ../cmake-build-release --target isl_benchmark -j 6

sudo cpupower frequency-set --governor performance
bench_csv InsertSparse
bench_csv InsertDense
bench_csv InsertRandom

bench_csv DeleteSparse
bench_csv DeleteDense
bench_csv DeleteRandom

bench_csv SearchSparse
bench_csv SearchDense
bench_csv SearchRandom
sudo cpupower frequency-set --governor powersave

rm -rf ../graphics
mkdir -p ../graphics
for fn in ../csv/*.csv; do
    base=$(basename -- "$fn")
    name="${base%.*}"
    python3 plot.py -f "${fn}" --output "../graphics/${name}.png"
done
