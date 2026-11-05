#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "uart_task.h"

// Tracking variables to verify that commands invoke the correct triggers
static int refresh_request_count = 0;
static int refresh_override_count = 0;

// Mock implementations of external functions called by console_parse_line
void request_epaper_refresh(void) {
    refresh_request_count++;
}

void override_epaper_refresh(void) {
    refresh_override_count++;
}

void setUp(void) {
    refresh_request_count = 0;
    refresh_override_count = 0;
}

void tearDown(void) {
    // Cleanup if needed
}

// ==========================================
// 1. CONSOLE PARSE LINE TESTS
// ==========================================

static void test_console_parse_line_refresh(void) {
    bool result = console_parse_line("refresh");

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(1, refresh_request_count);
    TEST_ASSERT_EQUAL(0, refresh_override_count);
}

static void test_console_parse_line_override(void) {
    bool result = console_parse_line("override");

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(0, refresh_request_count);
    TEST_ASSERT_EQUAL(1, refresh_override_count);
}

static void test_console_parse_line_unknown(void) {
    bool result = console_parse_line("unknown_command");

    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL(0, refresh_request_count);
    TEST_ASSERT_EQUAL(0, refresh_override_count);
}

static void test_console_parse_line_null(void) {
    bool result = console_parse_line(NULL);

    TEST_ASSERT_FALSE(result);
}


// ==========================================
// 2. CONSOLE PROCESS CHAR TESTS
// ==========================================

static void test_console_process_char_printable(void) {
    char buffer[32] = {0};
    size_t idx = 0;

    // Process a standard printable character 'a' (ASCII 97)
    bool submitted = console_process_char('a', buffer, sizeof(buffer), &idx);

    TEST_ASSERT_FALSE(submitted);
    TEST_ASSERT_EQUAL(1, idx);
    TEST_ASSERT_EQUAL_CHAR('a', buffer[0]);
    TEST_ASSERT_EQUAL_CHAR('\0', buffer[1]);
}

static void test_console_process_char_backspace(void) {
    char buffer[32] = "ab";
    size_t idx = 2;

    // Process backspace (\b)
    bool submitted = console_process_char('\b', buffer, sizeof(buffer), &idx);

    TEST_ASSERT_FALSE(submitted);
    TEST_ASSERT_EQUAL(1, idx);
    TEST_ASSERT_EQUAL_CHAR('a', buffer[0]);
}

static void test_console_process_char_enter_submits(void) {
    char buffer[32] = "refresh";
    size_t idx = 7;

    // Process ENTER (\n) to submit line
    bool submitted = console_process_char('\n', buffer, sizeof(buffer), &idx);

    TEST_ASSERT_TRUE(submitted);
    TEST_ASSERT_EQUAL(0, idx); // Index should reset after submit
    TEST_ASSERT_EQUAL_STRING("refresh", buffer);
}

static void test_console_process_char_enter_empty_ignored(void) {
    char buffer[32] = {0};
    size_t idx = 0;

    // Pressing ENTER on an empty buffer should not submit
    bool submitted = console_process_char('\n', buffer, sizeof(buffer), &idx);

    TEST_ASSERT_FALSE(submitted);
    TEST_ASSERT_EQUAL(0, idx);
}

static void test_console_process_char_eof_ignored(void) {
    char buffer[32] = {0};
    size_t idx = 0;

    bool submitted = console_process_char(EOF, buffer, sizeof(buffer), &idx);

    TEST_ASSERT_FALSE(submitted);
    TEST_ASSERT_EQUAL(0, idx);
}


// ==========================================
// MAIN TEST RUNNER
// ==========================================

int main(void) {
    UNITY_BEGIN();

    // Command Parsing Tests
    RUN_TEST(test_console_parse_line_refresh);
    RUN_TEST(test_console_parse_line_override);
    RUN_TEST(test_console_parse_line_unknown);
    RUN_TEST(test_console_parse_line_null);

    // Character Processing / Editing Tests
    RUN_TEST(test_console_process_char_printable);
    RUN_TEST(test_console_process_char_backspace);
    RUN_TEST(test_console_process_char_enter_submits);
    RUN_TEST(test_console_process_char_enter_empty_ignored);
    RUN_TEST(test_console_process_char_eof_ignored);

    return UNITY_END();
}

