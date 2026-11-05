#include "unity.h"
#include "display_epaper.h"
#include <string.h>

// External access to buffers defined in display_epaper.c
extern uint8_t bw_buffer[EPAPER_BUFFER_SIZE];
extern uint8_t red_buffer[EPAPER_BUFFER_SIZE];

void setUp(void) {
    // Clear buffers to a known state before each test
    epaper_clear_framebuffers(EPAPER_COLOR_WHITE);
}

void tearDown(void) {
    // Clean up after each test if needed
}

// ==========================================
// 1. FRAMEBUFFER CLEARING TESTS
// ==========================================

static void test_clear_framebuffers_white(void) {
    epaper_clear_framebuffers(EPAPER_COLOR_WHITE);

    // For White background: bw_buffer = 0xFF, red_buffer = 0xFF
    for (size_t i = 0; i < EPAPER_BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xFF, bw_buffer[i]);
        TEST_ASSERT_EQUAL_HEX8(0xFF, red_buffer[i]);
    }
}

static void test_clear_framebuffers_black(void) {
    epaper_clear_framebuffers(EPAPER_COLOR_BLACK);

    // For Black background: bw_buffer = 0x00, red_buffer = 0xFF (inactive red plane)
    for (size_t i = 0; i < EPAPER_BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x00, bw_buffer[i]);
        TEST_ASSERT_EQUAL_HEX8(0xFF, red_buffer[i]);
    }
}

static void test_clear_framebuffers_red(void) {
    epaper_clear_framebuffers(EPAPER_COLOR_RED);

    // For Red background: bw_buffer = 0xFF (inactive bw plane), red_buffer = 0x00
    for (size_t i = 0; i < EPAPER_BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xFF, bw_buffer[i]);
        TEST_ASSERT_EQUAL_HEX8(0x00, red_buffer[i]);
    }
}


// ==========================================
// 2. DRAWING SHAPES & BOX TESTS (HAPPY & EDGE)
// ==========================================

static void test_display_draw_box_happy_path(void) {
    // Draw a small 2x2 filled black box at origin (0,0)
    display_draw_box(0, 0, 2, 2, EPAPER_COLOR_BLACK, true);

    // Verify that the expected bits in the buffer changed from 0xFF (white)
    // Because (0,0) maps to specific rotated coordinates, we check that 
    // at least one byte has been modified (no longer 0xFF for BW plane).
    bool modified = false;
    for (size_t i = 0; i < EPAPER_BUFFER_SIZE; i++) {
        if (bw_buffer[i] != 0xFF) {
            modified = true;
            break;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(modified, "Drawing a black box should modify the black/white framebuffer.");
}

static void test_display_draw_box_out_of_bounds(void) {
    // Snapshot buffer state before out-of-bounds draw
    uint8_t bw_snapshot[EPAPER_BUFFER_SIZE];
    uint8_t red_snapshot[EPAPER_BUFFER_SIZE];
    memcpy(bw_snapshot, bw_buffer, EPAPER_BUFFER_SIZE);
    memcpy(red_snapshot, red_buffer, EPAPER_BUFFER_SIZE);

    // Attempt to draw completely outside screen dimensions (EPAPER_WIDTH x EPAPER_HEIGHT)
    display_draw_box(999, 999, 10, 10, EPAPER_COLOR_BLACK, true);
    display_draw_box(-50, -50, 10, 10, EPAPER_COLOR_RED, true);

    // Buffers should remain completely untouched
    TEST_ASSERT_EQUAL_MEMORY(bw_snapshot, bw_buffer, EPAPER_BUFFER_SIZE);
    TEST_ASSERT_EQUAL_MEMORY(red_snapshot, red_buffer, EPAPER_BUFFER_SIZE);
}


// ==========================================
// 3. STRING & GLYPH RENDERING EDGE CASES
// ==========================================

static void test_display_draw_string_null_font(void) {
    uint8_t bw_snapshot[EPAPER_BUFFER_SIZE];
    memcpy(bw_snapshot, bw_buffer, EPAPER_BUFFER_SIZE);

    // Passing a NULL font should safely return without crashing or altering memory
    display_draw_string(10, 10, "Hello", NULL, EPAPER_COLOR_BLACK);

    TEST_ASSERT_EQUAL_MEMORY(bw_snapshot, bw_buffer, EPAPER_BUFFER_SIZE);
}

static void test_display_draw_icon_null_glyph(void) {
    uint8_t bw_snapshot[EPAPER_BUFFER_SIZE];
    memcpy(bw_snapshot, bw_buffer, EPAPER_BUFFER_SIZE);

    // Passing a NULL glyph should safely return without crashing or altering memory
    display_draw_icon(10, 10, NULL, EPAPER_COLOR_BLACK);

    TEST_ASSERT_EQUAL_MEMORY(bw_snapshot, bw_buffer, EPAPER_BUFFER_SIZE);
}


// ==========================================
// MAIN TEST RUNNER
// ==========================================

int main(void) {
    UNITY_BEGIN();
    
    // Framebuffer clearing tests
    RUN_TEST(test_clear_framebuffers_white);
    RUN_TEST(test_clear_framebuffers_black);
    RUN_TEST(test_clear_framebuffers_red);
    
    // Box / Drawing tests
    RUN_TEST(test_display_draw_box_happy_path);
    RUN_TEST(test_display_draw_box_out_of_bounds);
    
    // Safety / Null pointer edge case tests
    RUN_TEST(test_display_draw_string_null_font);
    RUN_TEST(test_display_draw_icon_null_glyph);
    
    return UNITY_END();
}

