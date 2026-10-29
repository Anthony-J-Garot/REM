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

    // Labels. It's best to include the space at the end for proper sizing.
    const char *label_humidity_str = "Humidity: ";
    const char *label_pressure_str = "Pressure: ";
    const char *label_dew_point_str = "Dew Point: ";

    // Values
    char temperature_str[16] = "";
    char humidity_str[32];
    char pressure_str[32];
    char dew_point_str[32];

    // Build the values
    snprintf(temperature_str, sizeof(temperature_str), "%c %.1f%cF", '\x1E', temp_f, '\x1F');
    snprintf(humidity_str, sizeof(humidity_str),       "%.1f %%", humidity);
    snprintf(pressure_str, sizeof(pressure_str),       "%.0f inHg", pressure_inhg);
    snprintf(dew_point_str, sizeof(dew_point_str),     "%.1f%cF", dew_pt_h, '\x1F');

	int x_indent = 0;

    // Temperature shows in a large 20x32 font
    epaper_draw_string(LEFT_MARGIN, 10, temperature_str, &font_20x32, EPAPER_COLOR_BLACK);

    // Humidity
    epaper_draw_string(LEFT_MARGIN, 50, label_humidity_str, &font_8x8, EPAPER_COLOR_BLACK);
	x_indent = strlen(label_humidity_str) * font_8x8.width;
    epaper_draw_string(LEFT_MARGIN + x_indent, 50, humidity_str, &font_8x16, EPAPER_COLOR_BLACK);

    // Pressure
    epaper_draw_string(LEFT_MARGIN, 70, label_pressure_str, &font_8x8, EPAPER_COLOR_BLACK);
	x_indent = strlen(label_pressure_str) * font_8x8.width;
    epaper_draw_string(LEFT_MARGIN + x_indent, 70, pressure_str, &font_8x16, EPAPER_COLOR_BLACK);

    // Dew Point
    epaper_draw_string(LEFT_MARGIN, 90, label_dew_point_str, &font_8x8, EPAPER_COLOR_BLACK);
	x_indent = strlen(label_dew_point_str) * font_8x8.width;
    epaper_draw_string(LEFT_MARGIN + x_indent, 90, dew_point_str, &font_8x16, EPAPER_COLOR_BLACK);
}

