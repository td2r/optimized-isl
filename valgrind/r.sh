#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT="$(realpath "$SCRIPT_DIR/..")"
BIN="$(realpath "$ROOT/cmake-build-release")"

cd "$SCRIPT_DIR"

function add_run_line() {
    if [[ $# -ne 4 ]]; then
        echo "add_run_line called with $# arguments"
        echo "Usage: add_run_line FILENAME DATA_STRUCTURE DATA_TYPE SIZE"
        exit 1
    fi
    local fn="$1"
    local dstruct="$2"
    local dtype="$3"
    local dsize="$4"
    local runs=1
    if [[ "$dtype" == "Random" ]]; then
        runs=10
    fi
    echo -n "${dstruct}/${dsize}" >> "$fn"
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

function compare_structures() {
    if [[ $# -ne 3 ]]; then
        echo "compare_structures called with $# arguments"
        echo "Usage: compare_structures DATA_STRUCTURE1 DATA_STRUCTURE2 FOLDER"
        exit 1
    fi
    local dstruct1="$1"
    local dstruct2="$2"
    local fld="$3"
    mkdir -p "./$fld/png"
    for dtype in Sparse Dense Random
    do
        fn=$(realpath "./$fld/$dtype.in")
        truncate -s 0 "$fn"
        for dstruct in $dstruct1 $dstruct2
        do
            local max_size=100000
            if [[ "$dtype" == Random && "$dstruct" == CGAL ]]; then
                max_size=10000
            fi
            for (( dsize = 100; dsize <= $max_size; dsize *= 10 )) do
                add_run_line "$fn" "$dstruct" "$dtype" "$dsize"
            done
        done
        python3 "hist.py" "./$fld/$dtype.in" "./$fld/png/Mem${dtype}.png"
    done
}

# build release
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_MAKE_PROGRAM=ninja \
      -DCMAKE_CXX_COMPILER=g++ \
      -G Ninja \
      -S "$ROOT" \
      -B "$BIN"
cmake --build "$BIN" --target memory_usage -j 6

compare_structures Optimized CGAL cgal_cmp
compare_structures Optimized Cartesian cartesian_cmp

