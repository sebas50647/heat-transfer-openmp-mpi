#!/usr/bin/env python3
"""Renders bench/summary.json as a dual-axis speedup/efficiency SVG chart.
Hand-rolled (no matplotlib in this environment) but kept small and
self-contained so it can be re-run after future benchmark sweeps.
"""
import json

with open("bench/summary.json") as f:
    rows = json.load(f)  # [threads, avg_s, speedup, efficiency]

W, H = 720, 440
PAD_L, PAD_R, PAD_T, PAD_B = 70, 70, 40, 60
plot_w = W - PAD_L - PAD_R
plot_h = H - PAD_T - PAD_B

max_threads = max(r[0] for r in rows)
max_speedup = max(max(r[2] for r in rows), max_threads)


def x_of(threads):
    return PAD_L + (threads / max_threads) * plot_w


def y_speedup(speedup):
    return PAD_T + plot_h * (1 - speedup / max_speedup)


def y_efficiency(eff):
    return PAD_T + plot_h * (1 - eff)


speedup_pts = " ".join(f"{x_of(t):.1f},{y_speedup(s):.1f}" for t, _, s, _ in rows)
efficiency_pts = " ".join(f"{x_of(t):.1f},{y_efficiency(e):.1f}" for t, _, _, e in rows)
ideal_pts = f"{x_of(1):.1f},{y_speedup(1):.1f} {x_of(max_threads):.1f},{y_speedup(max_threads):.1f}"

svg = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" '
       f'viewBox="0 0 {W} {H}" font-family="Helvetica,Arial,sans-serif">']
svg.append(f'<rect width="{W}" height="{H}" fill="#ffffff"/>')

# Gridlines + left axis (speedup) ticks
for s in range(0, int(max_speedup) + 2, 2):
    y = y_speedup(s)
    svg.append(f'<line x1="{PAD_L}" y1="{y:.1f}" x2="{W-PAD_R}" y2="{y:.1f}" '
                f'stroke="#e5e7eb" stroke-width="1"/>')
    svg.append(f'<text x="{PAD_L-10}" y="{y+4:.1f}" text-anchor="end" '
                f'font-size="12" fill="#374151">{s}</text>')

# Right axis (efficiency) ticks
for e10 in range(0, 11, 2):
    e = e10 / 10
    y = y_efficiency(e)
    svg.append(f'<text x="{W-PAD_R+10}" y="{y+4:.1f}" text-anchor="start" '
                f'font-size="12" fill="#374151">{e:.1f}</text>')

# X axis ticks (thread counts)
for t, _, _, _ in rows:
    x = x_of(t)
    svg.append(f'<line x1="{x:.1f}" y1="{PAD_T}" x2="{x:.1f}" y2="{H-PAD_B}" '
                f'stroke="#f3f4f6" stroke-width="1"/>')
    svg.append(f'<text x="{x:.1f}" y="{H-PAD_B+20}" text-anchor="middle" '
                f'font-size="12" fill="#374151">{t}</text>')

# Axes
svg.append(f'<line x1="{PAD_L}" y1="{PAD_T}" x2="{PAD_L}" y2="{H-PAD_B}" stroke="#111827" stroke-width="1.5"/>')
svg.append(f'<line x1="{W-PAD_R}" y1="{PAD_T}" x2="{W-PAD_R}" y2="{H-PAD_B}" stroke="#111827" stroke-width="1.5"/>')
svg.append(f'<line x1="{PAD_L}" y1="{H-PAD_B}" x2="{W-PAD_R}" y2="{H-PAD_B}" stroke="#111827" stroke-width="1.5"/>')

# Ideal linear speedup reference
svg.append(f'<polyline points="{ideal_pts}" fill="none" stroke="#9ca3af" '
           f'stroke-width="1.5" stroke-dasharray="4,4"/>')

# Speedup line (solid, blue) + markers
svg.append(f'<polyline points="{speedup_pts}" fill="none" stroke="#2563eb" stroke-width="2.5"/>')
for t, _, s, _ in rows:
    svg.append(f'<circle cx="{x_of(t):.1f}" cy="{y_speedup(s):.1f}" r="4" fill="#2563eb"/>')

# Efficiency line (dashed, orange) + markers
svg.append(f'<polyline points="{efficiency_pts}" fill="none" stroke="#ea580c" '
           f'stroke-width="2.5" stroke-dasharray="6,3"/>')
for t, _, _, e in rows:
    svg.append(f'<circle cx="{x_of(t):.1f}" cy="{y_efficiency(e):.1f}" r="4" fill="#ea580c"/>')

# Labels
svg.append(f'<text x="{W/2:.1f}" y="{H-15}" text-anchor="middle" font-size="13" '
           f'fill="#111827">Threads</text>')
svg.append(f'<text x="18" y="{PAD_T-15}" font-size="13" fill="#2563eb">Speedup (left)</text>')
svg.append(f'<text x="{W-PAD_R-100}" y="{PAD_T-15}" font-size="13" fill="#ea580c">Efficiency (right)</text>')
svg.append(f'<text x="{W/2:.1f}" y="20" text-anchor="middle" font-size="15" '
           f'fill="#111827" font-weight="bold">OpenMP speedup and efficiency (800x800 plate, 72 steps)</text>')

svg.append('</svg>')

with open("Designs/openmp_speedup.svg", "w") as f:
    f.write("\n".join(svg))
print("wrote Designs/openmp_speedup.svg")
