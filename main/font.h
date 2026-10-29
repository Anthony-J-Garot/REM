#pragma once

#include <stdint.h>

typedef struct {
    const uint8_t *table;     // Pointer to the raw font glyph data array
    uint8_t width;            // Character width in pixels
    uint8_t height;           // Character height in pixels
    uint8_t start_char;       // ASCII value of the first character (e.g., 32)
    uint8_t end_char;         // ASCII value of the last character (e.g., 126)
} epaper_font_t;

typedef struct {
    const uint8_t *table;     // Pointer to the raw font glyph data array
    uint8_t width;            // Character width in pixels
    uint8_t height;           // Character height in pixels
} epaper_glyph_t;

extern const epaper_font_t font_8x8;
extern const epaper_font_t font_8x16;
extern const epaper_font_t font_20x32;

