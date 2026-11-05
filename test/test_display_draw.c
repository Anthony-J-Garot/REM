#include "unity.h"
#include "display_draw.h"

// Define the framebuffers directly in the test file
uint8_t bw_buffer[EPAPER_BUFFER_SIZE];
uint8_t red_buffer[EPAPER_BUFFER_SIZE];

void setUp(void) {
    // Initialize buffers to 0xFF (White background for e-paper)
    for (int i = 0; i < EPAPER_BUFFER_SIZE; i++) {
        bw_buffer[i] = 0xFF;
        red_buffer[i] = 0xFF;
    }
}

void tearDown(void) {
    // Clean up if needed
}

/* =========================================================================
 * 1. Tests for display_draw_box() [Happy Paths & Edge Cases]
 * ========================================================================= */

void test_display_draw_box_filled_black(void) {
    // Draw a small 2x2 filled black box at origin (0,0)
    display_draw_box(0, 0, 2, 2, EPAPER_COLOR_BLACK, true);

    // For black color: bw_buffer bit should be cleared (0), red_buffer bit set (1)
    // Let's verify that the buffers changed from the initial 0xFF state.
    // Since it modifies the frame buffer according to the rotation matrix, 
    // we can check that at least some bits changed or test specific known coordinates.
    
    bool modified = false;
    for (int i = 0; i < EPAPER_BUFFER_SIZE; i++) {
        if (bw_buffer[i] != 0xFF) {
            modified = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(modified);
}

void test_display_draw_box_outline_red(void) {
    // Draw a 4x4 red box outline
    display_draw_box(10, 10, 4, 4, EPAPER_COLOR_RED, false);

    bool red_modified = false;
    for (int i = 0; i < EPAPER_BUFFER_SIZE; i++) {
        if (red_buffer[i] != 0xFF) {
            red_modified = true;
            break;
        }
    }
    // Red color clears bits in the red_buffer
    TEST_ASSERT_TRUE(red_modified);
}

void test_display_draw_box_out_of_bounds_ignored(void) {
    // Draw a box completely off-screen; should not crash or modify buffers
    display_draw_box(500, 500, 10, 10, EPAPER_COLOR_BLACK, true);

    for (int i = 0; i < EPAPER_BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xFF, bw_buffer[i]);
        TEST_ASSERT_EQUAL_HEX8(0xFF, red_buffer[i]);
    }
}


/* =========================================================================
 * 2. Tests for display_draw_icon() [Happy Paths & Edge Cases]
 * ========================================================================= */

void test_display_draw_icon_null_glyph_safeguard(void) {
    // Passing a NULL glyph should safely return without crashing
    display_draw_icon(0, 0, NULL, EPAPER_COLOR_BLACK);
    TEST_ASSERT_EQUAL_HEX8(0xFF, bw_buffer[0]);
}

void test_display_draw_icon_null_table_safeguard(void) {
    epaper_glyph_t invalid_glyph = {
        .width = 8,
        .height = 8,
        .table = NULL
    };
    display_draw_icon(0, 0, &invalid_glyph, EPAPER_COLOR_BLACK);
    TEST_ASSERT_EQUAL_HEX8(0xFF, bw_buffer[0]);
}

void test_display_draw_icon_valid(void) {
    // Define a simple 8x1 pixel icon (single byte: 0x80 means first bit set)
    static const uint8_t icon_data[] = { 0x80 };
    epaper_glyph_t valid_glyph = {
        .width = 8,
        .height = 1,
        .table = icon_data
    };

    display_draw_icon(0, 0, &valid_glyph, EPAPER_COLOR_BLACK);

    // Verify buffer was altered
    bool modified = false;
    for (int i = 0; i < EPAPER_BUFFER_SIZE; i++) {
        if (bw_buffer[i] != 0xFF) {
            modified = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(modified);
}


/* =========================================================================
 * 3. Tests for display_draw_string() [Happy Paths & Edge Cases]
 * ========================================================================= */

void test_display_draw_string_null_font_safeguard(void) {
    // Passing a NULL font should safely return
    display_draw_string(0, 0, "Test", NULL, EPAPER_COLOR_BLACK);
    TEST_ASSERT_EQUAL_HEX8(0xFF, bw_buffer[0]);
}

void test_display_draw_string_valid(void) {
    // Create a mock font table for character 'A' (ASCII 65)
    // Let's say width = 8, height = 8 (1 byte per row, 8 rows total)
    static const uint8_t font_table[8] = {
        0x18, //   @@  
        0x24, //  @  @ 
        0x42, // @    @
        0x42, // @    @
        0x7E, // @@@@@@
        0x42, // @    @
        0x42, // @    @
        0x00  //        
    };

    epaper_font_t mock_font = {
        .width = 8,
        .height = 8,
        .start_char = 'A',
        .end_char = 'A',
        .table = font_table
    };

    display_draw_string(0, 0, "A", &mock_font, EPAPER_COLOR_BLACK);

    // Verify pixels got drawn
    bool modified = false;
    for (int i = 0; i < EPAPER_BUFFER_SIZE; i++) {
        if (bw_buffer[i] != 0xFF) {
            modified = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(modified);
}

void test_display_draw_string_out_of_bounds_fallback(void) {
    // Character outside range should trigger the fallback index 0 without crashing
    static const uint8_t font_table[8] = { 0xFF, 0, 0, 0, 0, 0, 0, 0 };

    epaper_font_t mock_font = {
        .width = 8,
        .height = 8,
        .start_char = 'B',
        .end_char = 'B',
        .table = font_table
    };

    // Printing 'A' falls outside 'B'–'B', invoking fallback to index 0
    display_draw_string(0, 0, "A", &mock_font, EPAPER_COLOR_BLACK);
    
    bool modified = false;
    for (int i = 0; i < EPAPER_BUFFER_SIZE; i++) {
        if (bw_buffer[i] != 0xFF) {
            modified = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(modified);
}


/* =========================================================================
 * Main Runner
 * ========================================================================= */

int main(void) {
    UNITY_BEGIN();

    // Box tests
    RUN_TEST(test_display_draw_box_filled_black);
    RUN_TEST(test_display_draw_box_outline_red);
    RUN_TEST(test_display_draw_box_out_of_bounds_ignored);

    // Icon tests
    RUN_TEST(test_display_draw_icon_null_glyph_safeguard);
    RUN_TEST(test_display_draw_icon_null_table_safeguard);
    RUN_TEST(test_display_draw_icon_valid);

    // String tests
    RUN_TEST(test_display_draw_string_null_font_safeguard);
    RUN_TEST(test_display_draw_string_valid);
    RUN_TEST(test_display_draw_string_out_of_bounds_fallback);

    return UNITY_END();
}
