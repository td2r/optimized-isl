#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT="$(realpath "$SCRIPT_DIR/..")"
BIN="$(realpath "$ROOT/cmake-build-release")"

cd "$SCRIPT_DIR"

function add_run_line() {
    local fn="$1"
    local dstruct="$2"
    local dtype="$3"
    local dsize="$4"
    local runs=1
    if [[ "$dtype" == "Random" ]]; then
        runs=10
    fi
    echo -n "${dstruct}${dtype}/${dsize}" >> "$fn"
    for (( i = 0; i < ${runs}; ++i )); do
        valgrind \
            --tool=massif \
            --massif-out-file=./massif.out \
            "$BIN/memory_usage" \
            "$dstruct"  "$dtype" "$dsize" \
            &> /dev/null
        bytes=$(grep mem_heap_B massif.out | \
            sed -e 's/mem_heap_B=\(.*\)/\1/' | \
            sort -g -r | \
            head -n 1)
        echo -n " $bytes" >> "$fn"
    done
    echo "" >> "$fn"
}

# build release
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_MAKE_PROGRAM=ninja \
      -DCMAKE_CXX_COMPILER=g++ \
      -G Ninja \
      -S "$ROOT" \
      -B "$BIN"
cmake --build "$BIN" --target memory_usage -j 6

mkdir -p ./run_data
for dtype in Sparse Dense Random
do
    fn=$(realpath "./run_data/${dtype}.in")
    truncate -s 0 "$fn"
    for dstruct in Optimized CGAL
    do
        for (( dsize = 100; dsize <= 100000; dsize *= 10 )) do
            add_run_line "$fn" "$dstruct" "$dtype" "$dsize"
        done
    done
    python3 "hist.py" "./run_data/$dtype.in" "../graphics/Mem${dtype}.png"
done

