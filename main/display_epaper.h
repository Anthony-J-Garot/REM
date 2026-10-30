#pragma once

#include "esp_err.h"
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

// Initialize GPIOs and SPI bus
esp_err_t epaper_init(void);

// Hardware control routines
void epaper_reset(void);
void epaper_wait_until_idle(void);

// Framebuffer management
void epaper_clear_framebuffers(epaper_color_t background);
void epaper_draw_pixel(int pixel_x, int pixel_y, epaper_color_t color);

// Updated string drawing function supporting any font struct
void epaper_draw_string(int pixel_x, int pixel_y, const char *text, const epaper_font_t *font, epaper_color_t color);
void epaper_draw_box(int pixel_x, int pixel_y, int width, int height, epaper_color_t color, bool filled);
void epaper_draw_glyph(int pixel_x, int pixel_y, const epaper_glyph_t *glyph, epaper_color_t color);

// Refresh routine to push buffers to the panel
void epaper_display_refresh(void);

// Framebuffer export APIs for PC inspection over UART
void epaper_export_bw_framebuffer_uart(void);
void epaper_export_red_framebuffer_uart(void);

