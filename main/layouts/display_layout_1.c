#include "display_formatter.h"
#include <stdio.h>
#include <string.h>
#include "display_epaper.h"
#include "glyph.h"

void display_formatter_draw(float temp_f, float pressure_inhg, float humidity, float dew_pt_h)
{
    // Format separate strings for components
    char temperature_str[16] = "";
    char humidity_str[32];
    char pressure_str[32];
    char dew_point_str[32];

    // Just jam the degree symbol next to the numbers. I purposely made it more to the right of the glyph.
    snprintf(temperature_str, sizeof(temperature_str), "%c %.1f%cF", '\x1E', temp_f, '\x1F');
    snprintf(humidity_str, sizeof(humidity_str),   "Humidity: %.1f %%", humidity);
    snprintf(pressure_str, sizeof(pressure_str),   "Pressure: %.0f inHg", pressure_inhg);
    snprintf(dew_point_str, sizeof(dew_point_str), "Dew Point: %.1f%cF", dew_pt_h, '\x1F');

    char header_str[64];
    snprintf(header_str, sizeof(header_str), "REM Weather Station");

    // Draw components with appropriate fonts and X offsets
    const int LEFT_MARGIN = 10;

    display_draw_string(LEFT_MARGIN, 10, header_str, &font_8x8, EPAPER_COLOR_RED);

    // Temperature shows in a large 20x32 font
    display_draw_string(LEFT_MARGIN, 30, temperature_str, &font_20x32, EPAPER_COLOR_BLACK);

    // Push humidity, pressure, and dew point down below the 32px height footprint
    display_draw_string(LEFT_MARGIN, 70, humidity_str, &font_8x8, EPAPER_COLOR_BLACK);
    display_draw_string(LEFT_MARGIN, 90, pressure_str, &font_8x8, EPAPER_COLOR_BLACK);
    display_draw_string(LEFT_MARGIN, 110, dew_point_str, &font_8x8, EPAPER_COLOR_BLACK);
}

