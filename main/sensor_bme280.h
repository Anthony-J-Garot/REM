#pragma once

#include "bme280.h" // Bosch
#include "driver/i2c_master.h"

#define I2C_MASTER_SCL_IO		10		// GPIO10 for SCL
#define I2C_MASTER_SDA_IO		11		// GPIO11 for SDA
#define I2C_MASTER_NUM			I2C_NUM_0
#define I2C_MASTER_FREQ_HZ		100000	// 100kHz standard speed
#define I2C_TRANSMIT_TIMEOUT	100		// ms

// Public initialization function
esp_err_t sensor_bme280_init(i2c_master_bus_handle_t bus_handle);

// Public function to grab sensor data cleanly
int8_t sensor_bme280_get_data(struct bme280_data *comp_data);

extern int8_t bme280_i2c_read(
	uint8_t reg_addr,
	uint8_t *reg_data,
	uint32_t len,
	void *intf_ptr);

// Bosch API Callback: I2C Write
extern int8_t bme280_i2c_write(
	uint8_t reg_addr,
	const uint8_t *reg_data,
	uint32_t len,
	void *intf_ptr);

// Bosch API Callback: Delay
extern void bme280_delay_us(uint32_t period, void *intf_ptr);

