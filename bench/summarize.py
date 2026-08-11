#!/usr/bin/env python3
"""Reads bench/results.csv (threads,run,seconds), prints a speedup/
efficiency table, and writes bench/summary.json for make_chart.py.
"""
import csv
import json
from collections import defaultdict

times = defaultdict(list)
with open("bench/results.csv") as f:
    for row in csv.DictReader(f):
        times[int(row["threads"])].append(float(row["seconds"]))

baseline = sum(times[1]) / len(times[1])

print(f"{'threads':>7} {'avg_s':>8} {'speedup':>8} {'efficiency':>10}")
rows = []
for threads in sorted(times):
    avg = sum(times[threads]) / len(times[threads])
    speedup = baseline / avg
    efficiency = speedup / threads
    rows.append((threads, avg, speedup, efficiency))
    print(f"{threads:>7} {avg:>8.3f} {speedup:>8.3f} {efficiency:>10.3f}")

with open("bench/summary.json", "w") as f:
    json.dump(rows, f)
