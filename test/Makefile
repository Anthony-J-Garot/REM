CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
CFLAGS += -DTESTING -D_GNU_SOURCE -DCONFIG_IDF_TARGET=\"esp32h2\"

# Output directory for test binaries
BIN_DIR = bin

INC_DIRS = -I. -Iunity -Imocks -I../main

UNITY_SRC = unity/unity.c

ESP_MOCKS_SRC = mocks/esp_mocks.c

# 1. Automatically find all test_*.c files in the directory
TEST_SRCS = $(wildcard test_*.c)

# 2. Derive executable paths inside bin/ (e.g., test_math_utils.c -> bin/test_math_utils)
TEST_BINARIES = $(patsubst %.c,$(BIN_DIR)/%,$(filter-out test_display_formatter.c,$(TEST_SRCS)))

.PHONY: all test clean

all: test

# Create the output directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $@

# Target rule to compile test_uart_task with debug console enabled
$(BIN_DIR)/test_uart_task: test_uart_task.c ../main/uart_task.c $(ESP_MOCKS_SRC) $(UNITY_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -DENABLE_DEBUG_CONSOLE $(INC_DIRS) $^ -o $@ -lm

# Target rule for test_display_epaper to include mocks/esp_mocks.c
$(BIN_DIR)/test_display_epaper: test_display_epaper.c ../main/display_epaper.c $(ESP_MOCKS_SRC) $(UNITY_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INC_DIRS) $^ -o $@ -lm

# Target rule to compile test_display_export with all required source dependencies
$(BIN_DIR)/test_display_export: test_display_export.c ../main/display_export.c ../main/display_epaper.c $(ESP_MOCKS_SRC) $(UNITY_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INC_DIRS) $^ -o $@ -lm

# General rule for other tests without hardware mocks
$(BIN_DIR)/test_%: test_%.c ../main/%.c $(UNITY_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INC_DIRS) $^ -o $@ -lm

# Target to run ALL tests sequentially from the bin/ directory
test: $(TEST_BINARIES)
	@echo "--- Running All Unit Tests ---"
	@for test in $(TEST_BINARIES); do \
		echo "\n>>> Executing $$test..."; \
		./$$test || exit 1; \
	done
	@echo "--- All Tests Passed! ---"

clean:
	@echo "Clearing build artifacts..."
	@rm -rf $(BIN_DIR)

