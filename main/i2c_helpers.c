#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "i2c_helpers.h"

static const char *TAG = "i2c_helpers";

int i2c_scanner_run(i2c_master_bus_handle_t bus_handle)
{
    ESP_LOGI(TAG, "Running I2C Scanner grid...");

    int devices_found = 0;

    printf("\n     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    for (int i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j++) {
            int addr = i + j;
            if (addr < 0x08 || addr > 0x77) {
                printf("   ");
                continue;
            }

            esp_err_t ret = i2c_master_probe(bus_handle, addr, 50);
            if (ret == ESP_OK) {
                printf("%02x ", addr);
                devices_found++;
            } else {
                printf("-- ");
            }
        }
        printf("\n");
    }
    printf("\n");

    ESP_LOGI(TAG, "Scan complete. Total devices found: %d", devices_found);
    return devices_found;
}

