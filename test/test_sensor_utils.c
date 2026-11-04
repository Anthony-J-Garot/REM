#include "unity.h"
#include "sensor_utils.h"

void setUp(void) {
    // Initialize before each test if needed
}

void tearDown(void) {
    // Clean up after each test if needed
}

/* --- Tests for c_to_f --- */

void test_c_to_f_freezing(void) {
    // 0°C should be 32°F
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 32.0f, c_to_f(0.0f));
}

void test_c_to_f_boiling(void) {
    // 100°C should be 212°F
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 212.0f, c_to_f(100.0f));
}

void test_c_to_f_negative(void) {
    // -40°C is famously -40°F
    TEST_ASSERT_FLOAT_WITHIN(0.01f, -40.0f, c_to_f(-40.0f));
}

void test_c_to_f_room_temp(void) {
    // 20°C should be 68°F
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 68.0f, c_to_f(20.0f));
}


/* --- Tests for hpa_to_inhg --- */

void test_hpa_to_inhg_standard_pressure(void) {
    // Standard atmosphere: 1013.25 hPa ~ 29.9213 inHg
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 29.9213f, hpa_to_inhg(1013.25f));
}

void test_hpa_to_inhg_zero(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, hpa_to_inhg(0.0f));
}

void test_hpa_to_inhg_arbitrary(void) {
    // 1000 hPa * 0.0295301 = 29.5301 inHg
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 29.5301f, hpa_to_inhg(1000.0f));
}


/* --- Tests for calculate_dew_point --- */

void test_calculate_dew_point_normal(void) {
    // Example: Temp = 20.0°C, Humidity = 50.0%
    float dew_point = calculate_dew_point(20.0f, 50.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 9.3f, dew_point);
}

void test_calculate_dew_point_high_humidity(void) {
    // Example: Temp = 25.0°C, Humidity = 100.0% (dew point equals temperature)
    float dew_point = calculate_dew_point(25.0f, 100.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 25.0f, dew_point);
}

void test_calculate_dew_point_zero_humidity_edge_case(void) {
    // Edge case safeguard: humidity <= 0.0 should return -40.0F
    float dew_point = calculate_dew_point(20.0f, 0.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -40.0f, dew_point);
}

void test_calculate_dew_point_negative_humidity_edge_case(void) {
    // Edge case safeguard: negative humidity should also return -40.0F
    float dew_point = calculate_dew_point(20.0f, -5.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -40.0f, dew_point);
}


/* --- Runner --- */

int main(void) {
    UNITY_BEGIN();

    // c_to_f tests
    RUN_TEST(test_c_to_f_freezing);
    RUN_TEST(test_c_to_f_boiling);
    RUN_TEST(test_c_to_f_negative);
    RUN_TEST(test_c_to_f_room_temp);

    // hpa_to_inhg tests
    RUN_TEST(test_hpa_to_inhg_standard_pressure);
    RUN_TEST(test_hpa_to_inhg_zero);
    RUN_TEST(test_hpa_to_inhg_arbitrary);

    // calculate_dew_point tests
    RUN_TEST(test_calculate_dew_point_normal);
    RUN_TEST(test_calculate_dew_point_high_humidity);
    RUN_TEST(test_calculate_dew_point_zero_humidity_edge_case);
    RUN_TEST(test_calculate_dew_point_negative_humidity_edge_case);

    return UNITY_END();
}
