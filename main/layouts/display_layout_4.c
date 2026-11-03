#include "display_formatter.h"
#include <stdio.h>
#include <string.h>
#include "display_epaper.h"
#include "glyph.h"

void display_formatter_draw(float temp_f, float pressure_inhg, float humidity, float dew_pt_h)
{
    const int LEFT_MARGIN = 10;

    // Values
    char temperature_str[16] = "";
    char humidity_str[16] = "";
    char pressure_str[16] = "";

	const char GLYPH_DEGREES = '\x1F';

    // Build the values
    snprintf(temperature_str, sizeof(temperature_str), "%.1f%cF", temp_f, GLYPH_DEGREES);
    snprintf(humidity_str, sizeof(humidity_str),       "%.1f %%", humidity);
    snprintf(pressure_str, sizeof(pressure_str),       "%.2f inHg", pressure_inhg);

    // Temperature 32x32 icon and values in a 20x32 font
    epaper_draw_glyph(LEFT_MARGIN, 10, &thermometer_empty_32x32, EPAPER_COLOR_BLACK);
    epaper_draw_glyph(LEFT_MARGIN, 10, &thermometer_mercury_32x32, EPAPER_COLOR_RED);
    epaper_draw_string(LEFT_MARGIN + thermometer_empty_32x32.width, 10, temperature_str, &font_20x32, EPAPER_COLOR_BLACK);

    // Humidity
    epaper_draw_glyph(LEFT_MARGIN, 50, &waterdrop_empty_32x32, EPAPER_COLOR_BLACK);
    epaper_draw_string(LEFT_MARGIN + waterdrop_empty_32x32.width, 50, humidity_str, &font_20x32, EPAPER_COLOR_BLACK);

    // Pressure
    epaper_draw_glyph(LEFT_MARGIN, 90, &dial_32x32, EPAPER_COLOR_BLACK);
    epaper_draw_string(LEFT_MARGIN + dial_32x32.width, 90, pressure_str, &font_20x32, EPAPER_COLOR_BLACK);
}

