/*
 * Removes red projecte name.
 * Introduces 8x16 font.
 */

#include "display_formatter.h"
#include <stdio.h>
#include <string.h>
#include "display_epaper.h"
#include "font.h"

void display_formatter_draw(float temp_f, float pressure_inhg, float humidity, float dew_pt_h)
{
    const int LEFT_MARGIN = 10;
	const int FONT_8X8_Y_OFFSET = 4;

    // Labels. It's best to include the space at the end for proper sizing.
    const char *label_humidity_str = "Humidity: ";
    const char *label_pressure_str = "Pressure: ";
    const char *label_dew_point_str = "Dew Point: ";

    // Values
    char temperature_str[16] = "";
    char humidity_str[32];
    char pressure_str[32];
    char dew_point_str[32];

	const char *GLYPH_THERMOMETER_MERCURY = "\x1C";
	const char GLYPH_THERMOMETER_EMPTY = '\x1D';
	const char GLYPH_DEGREES = '\x1F';

    // Build the values
    snprintf(temperature_str, sizeof(temperature_str), "%c %.1f%cF", GLYPH_THERMOMETER_EMPTY, temp_f, GLYPH_DEGREES);
    snprintf(humidity_str, sizeof(humidity_str),       "%.1f %%", humidity);
    snprintf(pressure_str, sizeof(pressure_str),       "%.0f inHg", pressure_inhg);
    snprintf(dew_point_str, sizeof(dew_point_str),     "%.1f%cF", dew_pt_h, GLYPH_DEGREES);

	int x_indent = 0;

    // Temperature shows in a large 20x32 font
    epaper_draw_string(LEFT_MARGIN, 10, temperature_str, &font_20x32, EPAPER_COLOR_BLACK);
    epaper_draw_string(LEFT_MARGIN, 10, GLYPH_THERMOMETER_MERCURY, &font_20x32, EPAPER_COLOR_RED);

    // Humidity
    epaper_draw_string(LEFT_MARGIN, 50 + FONT_8X8_Y_OFFSET, label_humidity_str, &font_8x8, EPAPER_COLOR_BLACK);
	x_indent = strlen(label_humidity_str) * font_8x8.width;
    epaper_draw_string(LEFT_MARGIN + x_indent, 50, humidity_str, &font_8x16, EPAPER_COLOR_BLACK);

    // Pressure
    epaper_draw_string(LEFT_MARGIN, 70 + FONT_8X8_Y_OFFSET, label_pressure_str, &font_8x8, EPAPER_COLOR_BLACK);
	x_indent = strlen(label_pressure_str) * font_8x8.width;
    epaper_draw_string(LEFT_MARGIN + x_indent, 70, pressure_str, &font_8x16, EPAPER_COLOR_BLACK);

    // Dew Point
    epaper_draw_string(LEFT_MARGIN, 90 + FONT_8X8_Y_OFFSET, label_dew_point_str, &font_8x8, EPAPER_COLOR_BLACK);
	x_indent = strlen(label_dew_point_str) * font_8x8.width;
    epaper_draw_string(LEFT_MARGIN + x_indent, 90, dew_point_str, &font_8x16, EPAPER_COLOR_BLACK);
}

