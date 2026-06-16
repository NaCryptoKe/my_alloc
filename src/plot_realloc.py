# /// script
# dependencies = ["pandas", "matplotlib"]
# ///
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

try:
    df = pd.read_csv("benchmark_results.csv")
except FileNotFoundError:
    print("Error: 'benchmark_results.csv' not found. Run your compiled binary first!")
    exit(1)

fig, ax = plt.subplots(figsize=(10, 6))

g_mean, g_p99, g_max, g_min = df["glibc_realloc"].mean(), df["glibc_realloc"].quantile(0.99), df["glibc_realloc"].max(), df["glibc_realloc"].min()
c_mean, c_p99, c_max, c_min = df["custom_realloc"].mean(), df["custom_realloc"].quantile(0.99), df["custom_realloc"].max(), df["custom_realloc"].min()

ax.plot(df["iteration"], df["glibc_realloc"], color="#7426d9", alpha=0.4, label="Glibc raw")
ax.plot(df["iteration"], df["custom_realloc"], color="#2dcc0d", alpha=0.4, label="Custom raw")

ax.plot(df["iteration"], df["glibc_realloc"].rolling(100, min_periods=1).mean(), color="#7426d9", linewidth=2, label="Glibc Trend")
ax.plot(df["iteration"], df["custom_realloc"].rolling(100, min_periods=1).mean(), color="#2dcc0d", linewidth=2, label="Custom Trend")

ax.set_title("Resizing Latency Evaluation (Realloc)", fontsize=14, fontweight="bold")
ax.set_xlabel("Iteration Index")
ax.set_ylabel("Latency (Nanoseconds)")
ax.grid(True, linestyle="--", alpha=0.5)

stats = f"GLIBC:\n Avg: {g_mean:.1f} ns\n P99: {g_p99:.1f} ns\n Worst: {g_max:.1f}\n Best: {g_min:.1f}\n\nCUSTOM:\n Avg: {c_mean:.1f} ns\n P99: {c_p99:.1f} ns\n Worst: {c_max:.1f}\n Best: {c_min:.1f}"
ax.text(0.75, 0.60, stats, transform=ax.transAxes, bbox=dict(facecolor='white', alpha=0.85, boxstyle='round'))

ax.legend(loc="upper left")
plt.tight_layout()
plt.savefig("report_realloc.svg", dpi=300)
print("Analysis report generated successfully: 'report_realloc.svg'")