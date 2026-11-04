#include <math.h>
#include "sensor_utils.h"

float c_to_f(float celsius) {
    return (celsius * 9.0F / 5.0F) + 32.0F;
}

float hpa_to_inhg(float hpa) {
    // 1 inHg = 33.86389 hPa (or multiply by ~0.02953)
    return hpa * 0.0295301F;
}

float calculate_dew_point(float temp_c, float humidity) {
    // Note: millibars (mb) and hPa are numerically identical (1 hPa = 1 mb), 
    // so no conversion is needed if you want millibars!

    // Using the Magnus-Tetens approximation formula
    if (humidity <= 0.0F) {
        return -40.0F; // Edge case safeguard
    }
    
    float coeff_a = 17.27F;
    float coeff_b = 237.7F;
    float alpha = ((coeff_a * temp_c) / (coeff_b + temp_c)) + logf(humidity / 100.0F);
    float dew_point = (coeff_b * alpha) / (coeff_a - alpha);
    
    return dew_point;
}

