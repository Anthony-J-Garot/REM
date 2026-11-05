#include "unity.h"
#include "display_epaper.h"

void setUp(void) {
    // Initialize before each test if needed
}

void tearDown(void) {
    // Clean up after each test if needed
}

static void test_epaper_stub(void) {
    // Simple stub to verify build setup and dependency linking
    TEST_ASSERT_TRUE(1);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_epaper_stub);
    return UNITY_END();
}

