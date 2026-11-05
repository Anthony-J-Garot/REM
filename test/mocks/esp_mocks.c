#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"

// FreeRTOS stub state
TickType_t g_mock_total_ticks_delayed = 0;

void vTaskDelay(const TickType_t xTicksToDelay) {
    g_mock_total_ticks_delayed += xTicksToDelay;
}

// GPIO stubs
esp_err_t gpio_config(const gpio_config_t *pGPIOConfig) {
    (void)pGPIOConfig;
    return ESP_OK;
}

esp_err_t gpio_set_level(uint32_t gpio_num, uint32_t level) {
    (void)gpio_num;
    (void)level;
    return ESP_OK;
}

int gpio_get_level(uint32_t gpio_num) {
    (void)gpio_num;
    return 0; // Default return state for pins (e.g., idle)
}

// SPI stubs
esp_err_t spi_bus_initialize(int host_id, const spi_bus_config_t *bus_config, int dma_chan) {
    (void)host_id;
    (void)bus_config;
    (void)dma_chan;
    return ESP_OK;
}

esp_err_t spi_bus_add_device(int host_id, const spi_device_interface_config_t *dev_config, spi_device_handle_t *handle) {
    (void)host_id;
    (void)dev_config;
    if (handle) {
        *handle = (spi_device_handle_t)0x1234; // Dummy non-NULL handle
    }
    return ESP_OK;
}

esp_err_t spi_device_polling_transmit(spi_device_handle_t handle, spi_transaction_t *trans_desc) {
    (void)handle;
    (void)trans_desc;
    return ESP_OK;
}

unsigned int uxTaskGetStackHighWaterMark(TaskHandle_t xTask) {
    (void)xTask;
    return 1024; // Mock headroom value well above warning thresholds
}

char *pcTaskGetName(TaskHandle_t xTask) {
    (void)xTask;
    return "mock_task";
}

