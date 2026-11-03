#include "display_draw.h"
#include "esp_log.h"

//static const char *TAG = "DRAW";

extern uint8_t bw_buffer[EPAPER_BUFFER_SIZE];
extern uint8_t red_buffer[EPAPER_BUFFER_SIZE];

// {{{ draw_pixel()
// This draws the individual pixels. While this can be called directly, it's used
// primarily to draw glyphs (font characters and icons) and shapes.
static void draw_pixel(int pixel_x, int pixel_y, epaper_color_t color)
{
	// Check bounds for landscape orientation, e.g., (296 x 128)
	if (pixel_x < 0 || pixel_x >= EPAPER_WIDTH || pixel_y < 0 || pixel_y >= EPAPER_HEIGHT) { return; }

	// Transpose coordinates: map landscape (x, y) to the panel's native portrait/scanned memory layout
	// For many WeAct 2.9\" panels, native RAM expects Y as the byte column and X as the bit/row stride.
	int rotated_x = pixel_y;
	int rotated_y = EPAPER_WIDTH - 1 - pixel_x;

	int addr = (rotated_x / 8) + (rotated_y * (EPAPER_HEIGHT / 8));
	int bit = 7 - (rotated_x % 8);

	if (addr < 0 || addr >= EPAPER_BUFFER_SIZE) { return; }

	if (color == EPAPER_COLOR_BLACK) {
		bw_buffer[addr] &= ~(1 << bit);
		red_buffer[addr] |= (1 << bit); 
	} else if (color == EPAPER_COLOR_RED) {
		bw_buffer[addr] |= (1 << bit);
		red_buffer[addr] &= ~(1 << bit); 
	} else { // White
		bw_buffer[addr] |= (1 << bit);
		red_buffer[addr] |= (1 << bit);
	}
}
// }}}

// {{{ draw_bitmap()
static void draw_bitmap(
    int pixel_x,
    int pixel_y,
    const uint8_t *data,
    int width,
    int height,
    epaper_color_t color)
{
    if (!data) { return; }

    int bytes_per_row = (width + 7) / 8;

    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            int byte_index = column / 8;
            int bit_index = 7 - (column % 8);

            if (data[(row * bytes_per_row) + byte_index] & (1 << bit_index)) {
                draw_pixel(pixel_x + column, pixel_y + row, color);
            }
        }
    }
}
// }}}

// {{{ display_draw_string()
// Puts text onto the display using the requested font.
void display_draw_string(int pixel_x, int pixel_y, const char *text, const epaper_font_t *font, epaper_color_t color)
{
    if (!font || !font->table) { return; }

    int bytes_per_row = (font->width + 7) / 8;

    while (*text) {
        char chr = *text++;
        int font_index = chr - font->start_char;
        
        if (font_index < 0 || font_index > (font->end_char - font->start_char)) {
            font_index = 0; // Fallback for out-of-bounds characters
        }

        // Calculate pointer to the specific character glyph bytes
        const uint8_t *glyph_data = font->table + ((ptrdiff_t)font_index * font->height * bytes_per_row);

        // Render the character using the common bitmap renderer
        draw_bitmap(
            pixel_x,
            pixel_y,
            glyph_data,
            font->width,
            font->height,
            color
        );

        pixel_x += font->width; // Advance character width dynamically
    }
}
// }}}

// {{{ display_draw_icon()
// Currently used only to draw icons.
void display_draw_icon(int pixel_x, int pixel_y, const epaper_glyph_t *glyph, epaper_color_t color)
{
    if (!glyph || !glyph->table) { return; }

    draw_bitmap(
        pixel_x,
        pixel_y,
        glyph->table,
        glyph->width,
        glyph->height,
        color
    );
}
// }}}

// {{{ display_draw_box()
// This function was used initially to solidity the drawing routines.
// Once that was figured out for this display, this function was no longer used.
void display_draw_box(int pixel_x, int pixel_y, int width, int height, epaper_color_t color, bool filled)
{
	for (int py = pixel_y; py < (pixel_y + height); py++) {
		for (int px = pixel_x; px < (pixel_x + width); px++) {
			if (filled || px == pixel_x || px == (pixel_x + width - 1) || py == pixel_y || py == (pixel_y + height - 1)) {
				draw_pixel(px, py, color);
			}
		}
	}
}
// }}}

