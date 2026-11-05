#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#define SPI2_HOST       0
#define SPI_DMA_CH_AUTO 0

typedef void* spi_device_handle_t;

typedef struct {
    int length;
    const void *tx_buffer;
    void *rx_buffer;
} spi_transaction_t;

typedef struct {
    int sclk_io_num;
    int mosi_io_num;
    int miso_io_num;
    int quadwp_io_num;
    int quadhd_io_num;
    int max_transfer_sz;
} spi_bus_config_t;

typedef struct {
    int clock_speed_hz;
    int mode;
    int spics_io_num;
    int queue_size;
} spi_device_interface_config_t;

// Mock function declarations
esp_err_t spi_bus_initialize(int host_id, const spi_bus_config_t *bus_config, int dma_chan);
esp_err_t spi_bus_add_device(int host_id, const spi_device_interface_config_t *dev_config, spi_device_handle_t *handle);
esp_err_t spi_device_polling_transmit(spi_device_handle_t handle, spi_transaction_t *trans_desc);

