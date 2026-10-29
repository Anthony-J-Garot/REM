#pragma once

#include <stdbool.h>

/**
 * @brief Renders the weather station content onto the e-paper display buffer.
 * 
 * @param temp_f Temperature in Fahrenheit.
 * @param pressure_inhg Pressure in inches of mercury (inHg).
 * @param humidity Relative humidity percentage.
 * @param dew_pt_h Dew point in Fahrenheit.
 */
void display_formatter_draw(float temp_f, float pressure_inhg, float humidity, float dew_pt_h);

