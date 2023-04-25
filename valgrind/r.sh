#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
CMAKE_RELEASE=$(realpath "$SCRIPT_DIR/../cmake-build-release")

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
            "$CMAKE_RELEASE"/memory_usage \
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

cmake --build "$CMAKE_RELEASE" --target memory_usage -j 6

mkdir -p ./run_data
for dtype in Sparse Dense Random
do
    fn=$(realpath "./run_data/${dtype}.in")
    truncate -s 0 "$fn"
    for dstruct in Optimized CGAL
    do
        for (( dsize = 100; dsize <= 10000; dsize *= 10 )) do
            add_run_line "$fn" "$dstruct" "$dtype" "$dsize"
        done
    done
done

