#pragma once
#include "FreeRTOS.h"

extern TickType_t g_mock_total_ticks_delayed;

void vTaskDelay(const TickType_t xTicksToDelay);
unsigned int uxTaskGetStackHighWaterMark(TaskHandle_t xTask);
char *pcTaskGetName(TaskHandle_t xTask);

