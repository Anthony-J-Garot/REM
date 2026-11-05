#pragma once

#include <stdbool.h>
#include "glyph.h" // Include font definitions

#define EPAPER_WIDTH  296
#define EPAPER_HEIGHT 128
#define EPAPER_BUFFER_SIZE ((EPAPER_WIDTH * EPAPER_HEIGHT) / 8)

// Color definitions for the BWR buffer
typedef enum {
    EPAPER_COLOR_WHITE = 0,
    EPAPER_COLOR_BLACK = 1,
    EPAPER_COLOR_RED   = 2
} epaper_color_t;

void display_draw_string(int pixel_x, int pixel_y, const char *text, const epaper_font_t *font, epaper_color_t color);
void display_draw_box(int pixel_x, int pixel_y, int width, int height, epaper_color_t color, bool filled);
void display_draw_icon(int pixel_x, int pixel_y, const epaper_glyph_t *glyph, epaper_color_t color);

