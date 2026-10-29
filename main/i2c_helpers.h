#pragma once

#include "driver/i2c_master.h"

/**
 * @brief Scans the I2C bus and prints a standard address grid.
 * 
 * @param bus_handle Initialized I2C master bus handle
 * @return int Total number of responding devices found
 */
int i2c_scanner_run(i2c_master_bus_handle_t bus_handle);

