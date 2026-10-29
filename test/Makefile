CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
CFLAGS += -DTESTING -DCONFIG_IDF_TARGET=\"esp32h2\"

# Output directory for test binaries
BIN_DIR = bin

INC_DIRS = -I. -Iunity -Imocks -I../main -I$(CJSON_DIR)

UNITY_SRC = unity/unity.c

# 1. Automatically find all test_*.c files in the directory
TEST_SRCS = $(wildcard test_*.c)

# 2. Derive executable paths inside bin/ (e.g., test_math_utils.c -> bin/test_math_utils)
TEST_BINARIES = $(patsubst %.c,$(BIN_DIR)/%,$(filter-out test_display_formatter.c,$(TEST_SRCS)))

.PHONY: all test clean

all: test

# 3. Create the output directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $@

# 4. General rule to build each individual test binary into bin/
# Uses an order-only prerequisite | $(BIN_DIR) so the directory is created before compiling
$(BIN_DIR)/test_%: test_%.c ../main/%.c $(UNITY_SRC) $(CJSON_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INC_DIRS) $^ -o $@ -lm

# 5. Target to run ALL tests sequentially from the bin/ directory
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

