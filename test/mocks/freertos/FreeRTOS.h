#pragma once
#include <stdint.h>

typedef uint32_t TickType_t;
typedef void * TaskHandle_t;

#define pdMS_TO_TICKS(x_ms) ((TickType_t)(x_ms))

