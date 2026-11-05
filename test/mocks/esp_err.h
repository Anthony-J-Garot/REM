#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int esp_err_t;

#define ESP_OK     0
#define ESP_FAIL   -1

// Host mock for ESP_ERROR_CHECK
#ifndef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x) do { \
    esp_err_t __err_rc = (x); \
    if (__err_rc != ESP_OK) { \
        fprintf(stderr, "ESP_ERROR_CHECK failed: %s at line %d (err=%d)\n", #x, __LINE__, __err_rc); \
        assert(__err_rc == ESP_OK); \
    } \
} while (0)
#endif
