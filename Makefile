# Compiler Configuration
CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -std=gnu11
LDFLAGS := 

# Directory Configuration
SRC_DIR := src
BIN_DIR := build

# Target Executable Names
TARGET_MAIN := main
TARGET_TEST := test


# Rules & Targets

# Default target: builds everything cleanly
all: $(BIN_DIR)/$(TARGET_MAIN) $(BIN_DIR)/$(TARGET_TEST)

# Build rule for the primary application
run: $(BIN_DIR)/$(TARGET_MAIN)
	@./$(BIN_DIR)/$(TARGET_MAIN)

# Build rule for the testing framework
test: $(BIN_DIR)/$(TARGET_TEST)
	@./$(BIN_DIR)/$(TARGET_TEST)

# Compilation recipe for the main binary
$(BIN_DIR)/$(TARGET_MAIN): $(SRC_DIR)/header.c $(SRC_DIR)/main.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compilation recipe for the stress-test binary
$(BIN_DIR)/$(TARGET_TEST): $(SRC_DIR)/header.c $(SRC_DIR)/test.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Direct directory generation rule to prevent missing-path faults
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Executes the Python/uv telemetry parsing layer
plot:
	uv run run_plots.py

# Complete cleanup rule
clean:
	rm -rf $(BIN_DIR)
	rm -f benchmark_results.csv report_*.svg

.PHONY: all run test plot clean