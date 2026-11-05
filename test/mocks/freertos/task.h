#pragma once
#include "FreeRTOS.h"

extern TickType_t g_mock_total_ticks_delayed;

void vTaskDelay(const TickType_t xTicksToDelay);

