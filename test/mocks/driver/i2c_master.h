#ifndef MOCK_DRIVER_I2C_MASTER_H
#define MOCK_DRIVER_I2C_MASTER_H

#include <stdint.h>
#include "esp_err.h"

// Provide minimal opaque handle types expected by esp_tmp117.h and related drivers
typedef struct i2c_master_bus_t *i2c_master_bus_handle_t;
typedef struct i2c_dev_t *i2c_master_dev_handle_t;

#endif // MOCK_DRIVER_I2C_MASTER_H

