#!/usr/bin/env bash
# Builds and runs the unit tests under tests/unit/. Each test is a
# standalone g++ binary (no MPI needed: Plate and Job don't depend on it).
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build_dir="$script_dir/../build/tests"
mkdir -p "$build_dir"

status=0
for test_src in "$script_dir"/unit/test_*.cpp; do
  test_name="$(basename "$test_src" .cpp)"
  test_bin="$build_dir/$test_name"

  extra_srcs=()
  case "$test_name" in
    test_job_io) extra_srcs=("$script_dir/../src/Job.cpp") ;;
  esac

  echo "== $test_name =="
  g++ -std=c++17 -Wall -Wextra "$test_src" "${extra_srcs[@]}" -o "$test_bin"
  if ! "$test_bin"; then
    status=1
  fi
  echo
done

exit "$status"
