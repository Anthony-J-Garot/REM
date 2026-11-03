#include "display_export.h"
#include "display_draw.h"
#include <stdio.h>
#include "esp_log.h"

//static const char *TAG = "EXPORT";

extern uint8_t bw_buffer[EPAPER_BUFFER_SIZE];
extern uint8_t red_buffer[EPAPER_BUFFER_SIZE];

// {{{ epaper_export_bw_framebuffer_uart()
// This dumps the black & white frame buffer to the JTAG/USB port as hexadecimal text.
// The results can be used by a companion python script to visualize the display as an
// image without actually rendering on the display. This gets around the 300 second
// manufacturer refresh minimum.
void epaper_export_bw_framebuffer_uart(void)
{
    // Print recognizable header with format, dimensions, and row-order convention
    printf("FRAMEBUFFER 296 128 XOR8 ROW_MAJOR_TOP_DOWN\n");

    for (int output_row = 0; output_row < EPAPER_HEIGHT; output_row++) {
        uint8_t row_bytes[37];
        memset(row_bytes, 0, sizeof(row_bytes));

        for (int output_column = 0; output_column < EPAPER_WIDTH; output_column++) {
            // Reverse engineer the coordinate mapping used in draw_pixel()
            int buffer_x = output_row;
            int buffer_y = EPAPER_WIDTH - 1 - output_column;

            int buffer_byte_index = (buffer_x / 8) + (buffer_y * (EPAPER_HEIGHT / 8));
            int buffer_bit_index = 7 - (buffer_x % 8);

            // bw_buffer: 0 = black, 1 = white
            if (buffer_byte_index >= 0 && buffer_byte_index < EPAPER_BUFFER_SIZE) {
                if (bw_buffer[buffer_byte_index] & (1 << buffer_bit_index)) {
                    row_bytes[output_column / 8] |= (1 << (7 - (output_column % 8)));
                }
            }
        }

        // Calculate 8-bit XOR checksum over the 37 decoded bytes
        uint8_t checksum = 0;
        for (int byte_index = 0; byte_index < 37; byte_index++) {
            checksum ^= row_bytes[byte_index];
        }

        // Output format: CC:HHHHHHHH... (74 hex characters for 37 bytes)
        printf("%02X:", checksum);
        for (int byte_index = 0; byte_index < 37; byte_index++) {
            printf("%02X", row_bytes[byte_index]);
        }
        printf("\n");
    }

    // Explicit end marker
    printf("END FRAMEBUFFER\n");
    fflush(stdout);
}
// }}}

// {{{ epaper_export_red_framebuffer_uart()
// See the note for epaper_export_bw_framebuffer_uart() for details.
void epaper_export_red_framebuffer_uart(void)
{
    // Print recognizable header for the red plane
    printf("FRAMEBUFFER_RED 296 128 XOR8 ROW_MAJOR_TOP_DOWN\n");

    for (int output_row = 0; output_row < EPAPER_HEIGHT; output_row++) {
        uint8_t row_bytes[37];
        memset(row_bytes, 0, sizeof(row_bytes));

        for (int output_column = 0; output_column < EPAPER_WIDTH; output_column++) {
            // Reverse engineer the coordinate mapping used in draw_pixel()
            int buffer_x = output_row;
            int buffer_y = EPAPER_WIDTH - 1 - output_column;

            int buffer_byte_index = (buffer_x / 8) + (buffer_y * (EPAPER_HEIGHT / 8));
            int buffer_bit_index = 7 - (buffer_x % 8);

            // red_buffer: 0 = red, 1 = white or black
            if (buffer_byte_index >= 0 && buffer_byte_index < EPAPER_BUFFER_SIZE) {
                if (red_buffer[buffer_byte_index] & (1 << buffer_bit_index)) {
                    row_bytes[output_column / 8] |= (1 << (7 - (output_column % 8)));
                }
            }
        }

        // Calculate 8-bit XOR checksum over the 37 decoded bytes
        uint8_t checksum = 0;
        for (int byte_index = 0; byte_index < 37; byte_index++) {
            checksum ^= row_bytes[byte_index];
        }

        // Output format: CC:HHHHHHHH...
        printf("%02X:", checksum);
        for (int byte_index = 0; byte_index < 37; byte_index++) {
            printf("%02X", row_bytes[byte_index]);
        }
        printf("\n");
    }

    // Explicit end marker
    printf("END FRAMEBUFFER_RED\n");
    fflush(stdout);
}
// }}}

