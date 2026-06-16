# /// script
# dependencies = ["pandas", "matplotlib"]
# ///
import subprocess
import sys
import os

scripts = [
    os.path.join("src", "plot_malloc.py"),
    os.path.join("src", "plot_free.py"),
    os.path.join("src", "plot_realloc.py"),
    os.path.join("src", "plot_calloc.py")
]

print("🚀 Starting allocator visual report generation...")

for script in scripts:
    print(f"→ Executing {script}...")
    # Calls 'uv run' on each script via Python's subprocess controller
    result = subprocess.run(["uv", "run", script], capture_output=True, text=True)
    
    if result.returncode == 0:
        print(f"Success: {script} output saved.")
    else:
        print(f"Error in {script}:", file=sys.stderr)
        print(result.stderr, file=sys.stderr)

print("\nAll benchmark reports generated successfully!")