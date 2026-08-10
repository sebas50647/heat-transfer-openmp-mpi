#!/usr/bin/env bash
# End-to-end tests: run the real heatsim binary (with MPI) against the
# fixtures in tests/fixtures/ and check every resulting plate###-k.bin
# against the matching file in tests/expected/ using bin/plate_diff.
# Complements tests/run_tests.sh, which covers Plate/Job unit logic but
# never exercises the simulator or the required comparison tool.
# Each fixture runs with both 1 and 2 MPI ranks to also catch
# job-distribution bugs, not just serial/OpenMP ones.
set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR" || exit 1

HEATSIM=bin/heat-transfer-openmp-mpi
PLATEDIFF=bin/plate_diff
EPSILON=0.000001
RANK_COUNTS="1 2"

if [ ! -x "$HEATSIM" ] || [ ! -x "$PLATEDIFF" ]; then
  echo "Build heatsim and plate_diff first (make)." >&2
  exit 1
fi

failures=0
total=0

for fixture in tests/fixtures/*/; do
  name="$(basename "$fixture")"
  expected_dir="tests/expected/$name"
  if [ ! -d "$expected_dir" ]; then
    echo "SKIP $name: no tests/expected/$name" >&2
    continue
  fi

  for ranks in $RANK_COUNTS; do
    total=$((total + 1))
    out_dir="$(mktemp -d)"
    mpirun --oversubscribe -np "$ranks" "$HEATSIM" \
      "$fixture/job.txt" "$fixture" "$out_dir" >"$out_dir/stdout.log" 2>&1

    case_failed=0
    for expected_file in "$expected_dir"/*.bin; do
      base="$(basename "$expected_file")"
      actual_file="$out_dir/$base"
      if [ ! -f "$actual_file" ]; then
        echo "FAIL $name (np=$ranks): missing output $base"
        case_failed=1
        continue
      fi
      if ! "$PLATEDIFF" "$actual_file" "$expected_file" "$EPSILON" \
          >"$out_dir/diff-$base.log" 2>&1; then
        echo "FAIL $name (np=$ranks): $base differs"
        cat "$out_dir/diff-$base.log"
        case_failed=1
      fi
    done

    if [ "$case_failed" -eq 0 ]; then
      echo "PASS $name (np=$ranks)"
      rm -rf "$out_dir"
    else
      echo "  see $out_dir for details"
      failures=$((failures + 1))
    fi
  done
done

echo "----"
echo "$((total - failures))/$total fixture runs passed"
exit $((failures > 0))
