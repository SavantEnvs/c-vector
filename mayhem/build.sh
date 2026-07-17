#!/usr/bin/env bash
#
# mayhem/build.sh — build the c-vector fuzz harness + the upstream test suite.
#
# c-vector is a header-only library (cvector.h / cvector_utils.h), so compiling the
# harness with $SANITIZER_FLAGS instruments the library code itself.
set -euo pipefail

[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH

: "${SANITIZER_FLAGS=-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer}"
: "${DEBUG_FLAGS:=-g -gdwarf-3}"
: "${CC:=clang}" ; : "${CXX:=clang++}" ; : "${LIB_FUZZING_ENGINE:=-fsanitize=fuzzer}"
: "${MAYHEM_JOBS:=$(nproc)}"
: "${COVERAGE_FLAGS=}"
export SANITIZER_FLAGS DEBUG_FLAGS CC CXX LIB_FUZZING_ENGINE MAYHEM_JOBS COVERAGE_FLAGS

cd "$SRC"

# 1+2) Fuzz harness (sanitized + instrumented; the header-only library is compiled into it)
#      and the standalone run-once reproducer.
$CC $SANITIZER_FLAGS $DEBUG_FLAGS $LIB_FUZZING_ENGINE -I"$SRC" \
    "$SRC/mayhem/fuzzer.c" -o /mayhem/c-vector-fuzzer
$CC $SANITIZER_FLAGS $DEBUG_FLAGS "$STANDALONE_FUZZ_MAIN" -I"$SRC" \
    "$SRC/mayhem/fuzzer.c" -o /mayhem/c-vector-fuzzer-standalone

# 3) Upstream test suite, with the project's NORMAL flags (independent clean build).
#    Both test binaries are EXCLUDE_FROM_ALL, so build them explicitly.
cmake -B "$SRC/build-tests" -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_FLAGS="$COVERAGE_FLAGS" -DCMAKE_EXE_LINKER_FLAGS="$COVERAGE_FLAGS" "$SRC"
cmake --build "$SRC/build-tests" -j"$MAYHEM_JOBS" --target test-c-vector unit-tests c-vector-example
