# /// script
# dependencies = ["pandas", "matplotlib"]
# ///
import pandas as pd
import matplotlib
# Force headless mode to silence interactive window warnings
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os

os.makedirs("plots", exist_ok=True)

# Load datasets
try:
    df = pd.read_csv("benchmark_results.csv")
except FileNotFoundError:
    print("Error: 'benchmark_results.csv' not found. Run your compiled binary first!")
    exit(1)

fig, ax = plt.subplots(figsize=(10, 6))

# Calculate summary metrics
g_mean, g_p99, g_max, g_min = df["glibc_calloc"].mean(), df["glibc_calloc"].quantile(0.99), df["glibc_calloc"].max(), df["glibc_calloc"].min()
c_mean, c_p99, c_max, c_min = df["custom_calloc"].mean(), df["custom_calloc"].quantile(0.99), df["custom_calloc"].max(), df["custom_calloc"].min()

# Plot raw individual metrics transparently in the background
ax.plot(df["iteration"], df["glibc_calloc"], color="#7426d9", alpha=0.4, label="Glibc raw")
ax.plot(df["iteration"], df["custom_calloc"], color="#2dcc0d", alpha=0.4, label="Custom raw")

# Plot macro trend paths (Fixed with min_periods=1 to span the full width of the data)
ax.plot(df["iteration"], df["glibc_calloc"].rolling(100, min_periods=1).mean(), color="#7426d9", linewidth=2, label="Glibc Trend")
ax.plot(df["iteration"], df["custom_calloc"].rolling(100, min_periods=1).mean(), color="#2dcc0d", linewidth=2, label="Custom Trend")

# Labels, layout engineering, and grid alignment
ax.set_title("Zeroed Allocation Latency Evaluation (Calloc)", fontsize=14, fontweight="bold")
ax.set_xlabel("Iteration Index")
ax.set_ylabel("Latency (Nanoseconds)")
ax.grid(True, linestyle="--", alpha=0.5)

# Format and pin summary statistics cards
stats = f"GLIBC:\n Avg: {g_mean:.1f} ns\n P99: {g_p99:.1f} ns\n Worst: {g_max:.1f}\n Best: {g_min:.1f}\n\nCUSTOM:\n Avg: {c_mean:.1f} ns\n P99: {c_p99:.1f} ns\n Worst: {c_max:.1f}\n Best: {c_min:.1f}"
ax.text(0.75, 0.60, stats, transform=ax.transAxes, bbox=dict(facecolor='white', alpha=0.85, boxstyle='round'))

ax.legend(loc="upper left")
plt.tight_layout()

# Save the artifact output directly to disk
output_path = os.path.join("plots", "report_calloc.svg")
plt.savefig(output_path, dpi=300)
print("Analysis report generated successfully: 'report_calloc.svg'")