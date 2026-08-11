#!/usr/bin/env bash
# Times bin/heat-transfer-openmp-mpi (release build, 1 MPI process) against
# bench/job.txt across thread counts, 3 repetitions each, and writes a CSV
# of the results to stdout: threads,run,seconds
set -u
cd "$(dirname "${BASH_SOURCE[0]}")/.." || exit 1

HEATSIM=bin/heat-transfer-openmp-mpi
JOB=bench/job.txt
INPUT_DIR=bench
THREAD_COUNTS="1 2 3 4 6 8 12"
REPS=3

python3 bench/generate_plate.py bench/plate.bin 800 >&2
printf 'plate.bin 1 0.2 1 0.5\n' > "$JOB"

echo "threads,run,seconds"
for threads in $THREAD_COUNTS; do
  for run in $(seq 1 "$REPS"); do
    out_dir="$(mktemp -d)"
    start=$(date +%s.%N)
    mpirun --oversubscribe -np 1 "$HEATSIM" "$JOB" "$INPUT_DIR" "$out_dir" "$threads" \
      >/dev/null 2>&1
    end=$(date +%s.%N)
    echo "$threads,$run,$(echo "$end - $start" | bc)"
    rm -rf "$out_dir"
  done
done
