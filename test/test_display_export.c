#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "display_export.h"
#include "display_epaper.h"
#include "display_draw.h"

// External access to buffers defined in the driver
extern uint8_t bw_buffer[EPAPER_BUFFER_SIZE];
extern uint8_t red_buffer[EPAPER_BUFFER_SIZE];

static char temp_filename[] = "test_stdout_capture.txt";

void setUp(void) {
    // Clear buffers before each test
    memset(bw_buffer, 0xFF, EPAPER_BUFFER_SIZE);   // All white
    memset(red_buffer, 0xFF, EPAPER_BUFFER_SIZE);  // All white
}

void tearDown(void) {
    // Clean up temporary capture file if it exists
    remove(temp_filename);
}

static int saved_stdout_fd = -1;
static int pipe_fds[2] = {-1, -1};

static void begin_capture(void) {
    fflush(stdout);
    
    // Save original stdout file descriptor
    saved_stdout_fd = dup(fileno(stdout));
    
    // Create a POSIX pipe for capturing output
    if (pipe(pipe_fds) == 0) {
        // Redirect stdout (fd 1) to the write-end of the pipe
        dup2(pipe_fds[1], fileno(stdout));
        close(pipe_fds[1]); // Close unused write descriptor reference
    }
}

static void end_capture(char *output_buf, size_t buf_size) {
    // Flush any remaining buffered output to stdout/pipe
    fflush(stdout);
    
    // Restore original stdout
    dup2(saved_stdout_fd, fileno(stdout));
    close(saved_stdout_fd);
    saved_stdout_fd = -1;

    // Read everything that was written into the pipe's read-end
    memset(output_buf, 0, buf_size);
    ssize_t total_read = 0;
    if (pipe_fds[0] != -1) {
        total_read = read(pipe_fds[0], output_buf, buf_size - 1);
        if (total_read > 0) {
            output_buf[total_read] = '\0';
        }
        close(pipe_fds[0]);
        pipe_fds[0] = -1;
    }
}


// ==========================================
// 1. HAPPY PATH TESTS
// ==========================================

static void test_export_bw_framebuffer_happy_path(void) {
    char capture_buffer[16384];

    epaper_clear_framebuffers(EPAPER_COLOR_WHITE);

    begin_capture();
    epaper_export_bw_framebuffer_uart();
    end_capture(capture_buffer, sizeof(capture_buffer));

    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(capture_buffer, "FRAMEBUFFER 296 128 XOR8 ROW_MAJOR_TOP_DOWN"), "Header missing");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(capture_buffer, "END FRAMEBUFFER"), "End marker missing");
}

static void test_export_red_framebuffer_happy_path(void) {
    char capture_buffer[16384];

    epaper_clear_framebuffers(EPAPER_COLOR_WHITE);

    begin_capture();
    epaper_export_red_framebuffer_uart();
    end_capture(capture_buffer, sizeof(capture_buffer));

    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(capture_buffer, "FRAMEBUFFER_RED 296 128 XOR8 ROW_MAJOR_TOP_DOWN"), "Red header missing");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(capture_buffer, "END FRAMEBUFFER_RED"), "Red end marker missing");
}


// ==========================================
// 2. EDGE CASE TESTS (DATA MODIFICATION & CHECKSUMS)
// ==========================================

static void test_export_framebuffer_with_black_pixels(void) {
    char capture_buffer_all_white[16384];
    char capture_buffer_modified[16384];

    epaper_clear_framebuffers(EPAPER_COLOR_WHITE);
    begin_capture();
    epaper_export_bw_framebuffer_uart();
    end_capture(capture_buffer_all_white, sizeof(capture_buffer_all_white));

    epaper_clear_framebuffers(EPAPER_COLOR_BLACK);
    begin_capture();
    epaper_export_bw_framebuffer_uart();
    end_capture(capture_buffer_modified, sizeof(capture_buffer_modified));

    TEST_ASSERT_NOT_EQUAL(0, strcmp(capture_buffer_all_white, capture_buffer_modified));
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(capture_buffer_modified, "END FRAMEBUFFER"), "Modified end marker missing");
}

static void test_export_red_framebuffer_with_red_pixels(void) {
    char capture_buffer[16384];

    epaper_clear_framebuffers(EPAPER_COLOR_RED);

    begin_capture();
    epaper_export_red_framebuffer_uart();
    end_capture(capture_buffer, sizeof(capture_buffer));

    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(capture_buffer, "FRAMEBUFFER_RED 296 128 XOR8 ROW_MAJOR_TOP_DOWN"), "Red pixel header missing");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(capture_buffer, "END FRAMEBUFFER_RED"), "Red pixel end marker missing");
}

// ==========================================
// MAIN TEST RUNNER
// ==========================================

int main(void) {
    UNITY_BEGIN();

    // Happy Path Tests
    RUN_TEST(test_export_bw_framebuffer_happy_path);
    RUN_TEST(test_export_red_framebuffer_happy_path);

    // Edge Case / Data Variation Tests
    RUN_TEST(test_export_framebuffer_with_black_pixels);
    RUN_TEST(test_export_red_framebuffer_with_red_pixels);

    return UNITY_END();
}
