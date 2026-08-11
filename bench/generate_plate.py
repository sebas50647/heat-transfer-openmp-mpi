#!/usr/bin/env python3
"""Writes a synthetic square plate (border=border, interior=interior)
in heatsim's binary format, for use as a benchmark/profiling dataset.
"""
import argparse
import struct


def write_plate(path, rows, cols, border, interior):
    with open(path, "wb") as f:
        f.write(struct.pack("<Q", rows))
        f.write(struct.pack("<Q", cols))
        for r in range(rows):
            for c in range(cols):
                is_border = r == 0 or r == rows - 1 or c == 0 or c == cols - 1
                f.write(struct.pack("<d", border if is_border else interior))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output")
    parser.add_argument("size", type=int, help="rows and columns (square plate)")
    parser.add_argument("--border", type=float, default=100.0)
    parser.add_argument("--interior", type=float, default=0.0)
    args = parser.parse_args()
    write_plate(args.output, args.size, args.size, args.border, args.interior)
    print(f"wrote {args.output}: {args.size}x{args.size}")
