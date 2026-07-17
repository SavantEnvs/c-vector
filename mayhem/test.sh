#!/usr/bin/env bash
#
# mayhem/test.sh — RUN the upstream test suite that mayhem/build.sh already built:
#   * build-tests/unit-tests    — upstream unit-tests.c (utest.h, 18 asserting test cases)
#   * build-tests/test-c-vector — upstream test.c (assert()-based memory-check test)
# No compilation happens here.
set -uo pipefail
[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH
cd "$SRC"

emit_ctrf() {
  local tool="$1" passed="$2" failed="$3" skipped="${4:-0}" pending="${5:-0}" other="${6:-0}"
  local tests=$(( passed + failed + skipped + pending + other ))
  cat > "${CTRF_REPORT:-$SRC/ctrf-report.json}" <<JSON
{
  "results": {
    "tool": { "name": "$tool" },
    "summary": {
      "tests": $tests,
      "passed": $passed,
      "failed": $failed,
      "pending": $pending,
      "skipped": $skipped,
      "other": $other
    }
  }
}
JSON
  printf 'CTRF {"results":{"tool":{"name":"%s"},"summary":{"tests":%d,"passed":%d,"failed":%d,"pending":%d,"skipped":%d,"other":%d}}}\n' \
    "$tool" "$tests" "$passed" "$failed" "$pending" "$skipped" "$other"
  [ "$failed" -eq 0 ]
}

for bin in build-tests/unit-tests build-tests/test-c-vector; do
  if [ ! -x "$bin" ]; then
    echo "FATAL: $bin missing — mayhem/build.sh must build it (do not compile here)" >&2
    emit_ctrf "utest" 0 1
    exit 1
  fi
done

passed=0
failed=0

# utest.h runner: prints "[  PASSED  ] N tests" and "[  FAILED  ] N tests, listed below:"
unit_out="$(./build-tests/unit-tests 2>&1)"
unit_rc=$?
echo "$unit_out"
unit_total="$(echo "$unit_out" | sed -n 's/^\[==========\] \([0-9][0-9]*\) test cases ran\..*/\1/p' | tail -1)"
unit_passed="$(echo "$unit_out" | sed -n 's/^\[  PASSED  \] \([0-9][0-9]*\) tests*.*/\1/p' | tail -1)"
if [ -z "$unit_total" ] || [ -z "$unit_passed" ]; then
  echo "FATAL: could not parse utest output" >&2
  emit_ctrf "utest" 0 1
  exit 1
fi
unit_failed=$(( unit_total - unit_passed ))
[ "$unit_rc" -eq 0 ] || [ "$unit_failed" -gt 0 ] || unit_failed=1
passed=$(( passed + unit_passed ))
failed=$(( failed + unit_failed ))

# assert()-based behavioral test: aborts (non-zero) on any wrong value
if ./build-tests/test-c-vector > /tmp/test-c-vector.out 2>&1; then
  passed=$(( passed + 1 ))
else
  echo "test-c-vector FAILED:" >&2
  cat /tmp/test-c-vector.out >&2
  failed=$(( failed + 1 ))
fi

emit_ctrf "utest" "$passed" "$failed"
