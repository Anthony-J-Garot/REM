#pragma once

#include <stdint.h>

typedef struct {
    const uint8_t *table;     // Pointer to bitmap data
    uint8_t width;            // Width in pixels
    uint8_t height;           // Height in pixels
} epaper_glyph_t;

typedef struct {
    const uint8_t *table;     // Pointer to font bitmap data
    uint8_t width;            // Character width in pixels
    uint8_t height;           // Character height in pixels
    uint8_t start_char;       // ASCII value of the first character
    uint8_t end_char;         // ASCII value of the last character
} epaper_font_t;

//extern const epaper_font_t font_8x8;
//extern const epaper_font_t font_8x16;
extern const epaper_font_t font_20x32;

extern const epaper_glyph_t thermometer_mercury_32x32;
extern const epaper_glyph_t thermometer_empty_32x32;
//extern const epaper_glyph_t thermometer_filled_32x32;

extern const epaper_glyph_t waterdrop_empty_32x32;
//extern const epaper_glyph_t waterdrop_filled_32x32;

extern const epaper_glyph_t dial_32x32;

