#pragma once

/**
 * @brief Converts Celsius to Fahrenheit.
 */
float c_to_f(float celsius);

/**
 * @brief Converts hectopascals (hPa) to inches of Mercury (inHg).
 */
float hpa_to_inhg(float hpa);

/**
 * @brief Calculates the dew point using the Magnus formula.
 * 
 * @param temp_c Temperature in Celsius
 * @param humidity Relative humidity in percentage (0-100)
 * @return float Dew point in Celsius
 */
float calculate_dew_point(float temp_c, float humidity);

