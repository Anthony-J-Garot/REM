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
	const int ICON_WIDTH = thermometer_empty_32x32.width; // Assume all icons are same size
	const int ICON_OFFSET = 5; // Can be positive or negative.

	// Build the values. Choosing the precision was tricky because the manufacturer's
	// accuracies are not great for the bme280.
	//		| Measurement	| Manufacturer specification					|
	//		| -------------	| --------------------------------------------- |
	//		| Temperature	| ±1.0°C = ±1.8°F								|
	//		| Humidity		| ±3% RH					 					|
	//		| Pressure		| ±1.0 hPa absolute accuracy (about 0.03 inHg)	|
	// I finalized on 1 digit of accuracy for each measurement for appearance sake.
	snprintf(temperature_str, sizeof(temperature_str), "%.1f%cF", temp_f, GLYPH_DEGREES);
	snprintf(humidity_str, sizeof(humidity_str), "%.1f %%", humidity);
	snprintf(pressure_str, sizeof(pressure_str), "%.1f inHg", pressure_inhg);

	// Temperature 32x32 icon and values in a 20x32 font
	display_draw_icon(LEFT_MARGIN, 10, &thermometer_empty_32x32, EPAPER_COLOR_BLACK);
	display_draw_icon(LEFT_MARGIN, 10, &thermometer_mercury_32x32, EPAPER_COLOR_RED);
	display_draw_string(LEFT_MARGIN + ICON_WIDTH + ICON_OFFSET, 10, 
		temperature_str, &font_20x32, EPAPER_COLOR_BLACK);

	// Humidity
	display_draw_icon(LEFT_MARGIN, 50, &waterdrop_empty_32x32, EPAPER_COLOR_BLACK);
	display_draw_string(LEFT_MARGIN + ICON_WIDTH + ICON_OFFSET, 50, 
		humidity_str, &font_20x32, EPAPER_COLOR_BLACK);

	// Pressure
	display_draw_icon(LEFT_MARGIN, 90, &dial_32x32, EPAPER_COLOR_BLACK);
	display_draw_string(LEFT_MARGIN + ICON_WIDTH + ICON_OFFSET, 90, 
		pressure_str, &font_20x32, EPAPER_COLOR_BLACK);
}

