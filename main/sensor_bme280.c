#include "sensor_bme280.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_helpers.h"

static const char *TAG = "BME280";

// Global handle for the I2C device used in callback functions
static i2c_master_dev_handle_t bme280_dev_handle = NULL;
static struct bme280_dev bme;

// {{{ configure_sensor_device()
static esp_err_t configure_sensor_device(i2c_master_bus_handle_t bus_handle) {

	// Register BME280 device on the bus (using address 0x76 confirmed earlier)
	i2c_device_config_t dev_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = 0x76,
		.scl_speed_hz = I2C_MASTER_FREQ_HZ,
	};
	ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &bme280_dev_handle));

	// Allow the sensor time to complete its power-on startup
	vTaskDelay(pdMS_TO_TICKS(200));

	// Initialize Bosch BME280 Struct Configuration
	bme.intf = BME280_I2C_INTF;
	bme.read = bme280_i2c_read;
	bme.write = bme280_i2c_write;
	bme.delay_us = bme280_delay_us;
	bme.intf_ptr = (void *) &bme280_dev_handle;

	// Initialize sensor hardware and load calibration trimming parameters
	int8_t rslt = bme280_init(&bme);
	if (rslt != BME280_OK) {
		ESP_LOGE(TAG, "Failed to initialize BME280 sensor! Error code: %d", rslt);
		return ESP_FAIL;
	}

	ESP_LOGI(TAG, "BME280 initialized successfully with official driver.");
	return ESP_OK;
}
// }}}

// {{{ configure_sensor_settings()
static esp_err_t configure_sensor_settings(void) {

	// Configure Sensor Settings using Bosch's official selection macros
	struct bme280_settings settings;
	settings.osr_h = BME280_OVERSAMPLING_1X;
	settings.osr_p = BME280_OVERSAMPLING_16X;
	settings.osr_t = BME280_OVERSAMPLING_2X;
	settings.filter = BME280_FILTER_COEFF_16;
	
	// Correct selection macro names for the official driver API
	uint8_t settings_sel = BME280_SEL_OSR_HUM | BME280_SEL_OSR_PRESS | BME280_SEL_OSR_TEMP | BME280_SEL_FILTER;
	int8_t rslt = bme280_set_sensor_settings(settings_sel, &settings, &bme);
	if (rslt != BME280_OK) {
		ESP_LOGE(TAG, "Failed to set sensor settings: %d", rslt);
		return ESP_FAIL;
	}

	// Set to Normal Mode using the correct power mode macro
	rslt = bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &bme);
	if (rslt != BME280_OK) {
		ESP_LOGE(TAG, "Failed to set sensor mode: %d", rslt);
		return ESP_FAIL;
	}

	return ESP_OK;
}
// }}}

// {{{ verify_sensor_registers()
// Read back configuration registers using bme280_i2c_read()
static esp_err_t verify_sensor_registers(void) {

	uint8_t reg_data = 0;

	// Read humidity oversampling (0xF2)
	if (bme280_i2c_read(0xF2, &reg_data, 1, bme.intf_ptr) == BME280_OK) {
		ESP_LOGI(TAG, "Reg 0xF2 (Humidity OSR): 0x%02X", reg_data);
	}

	// Read temp/press oversampling and mode (0xF4)
	if (bme280_i2c_read(0xF4, &reg_data, 1, bme.intf_ptr) == BME280_OK) {
		ESP_LOGI(TAG, "Reg 0xF4 (Temp/Press OSR & Mode): 0x%02X", reg_data);
	}

	// Read filter and standby config (0xF5)
	if (bme280_i2c_read(0xF5, &reg_data, 1, bme.intf_ptr) == BME280_OK) {
		ESP_LOGI(TAG, "Reg 0xF5 (Filter & Standby): 0x%02X", reg_data);
	}

	// Check status register (0xF3)
	const uint8_t STATUS_REGISTER = 0xF3;
	uint8_t status = 0;
	esp_err_t err = i2c_master_transmit_receive(
		bme280_dev_handle,
		&STATUS_REGISTER, 1,
		&status, 1,
		I2C_TRANSMIT_TIMEOUT);

	if (err == ESP_OK) {
		ESP_LOGI(TAG, "BME280 status: 0x%02X (measuring=%u, im_update=%u)",
			 	status, (status >> 3) & 1, status & 1);
	} else {
		ESP_LOGE(TAG, "Status read failed: %s", esp_err_to_name(err));
		return ESP_FAIL;
	}

	return ESP_OK;
}
// }}}

// {{{ sensor_bme280_init()
// Main Clean Initialization Orchestrator
esp_err_t sensor_bme280_init(i2c_master_bus_handle_t bus_handle) {
	ESP_ERROR_CHECK(configure_sensor_device(bus_handle));
	ESP_ERROR_CHECK(configure_sensor_settings());
	ESP_ERROR_CHECK(verify_sensor_registers());
	return ESP_OK;
}
// }}}

// {{{ sensor_bme280_get_data()
int8_t sensor_bme280_get_data(struct bme280_data *sensor_data) {
	int8_t rslt = bme280_get_sensor_data(BME280_ALL, sensor_data, &bme);
	if (rslt != BME280_OK) {
		ESP_LOGE(TAG, "Failed to stream sensor data: %d", rslt);
	}
	return rslt;
}
// }}}

// {{{ bme280_i2c_read()
int8_t bme280_i2c_read(
	uint8_t reg_addr,
	uint8_t *reg_data,
	uint32_t len,
	void *intf_ptr)
{
	i2c_master_dev_handle_t dev_handle =
		*(i2c_master_dev_handle_t *)intf_ptr;

	// Perform an atomic write-then-read with a Repeated Start (no intermediate STOP)
	esp_err_t err = i2c_master_transmit_receive(
		dev_handle,
		&reg_addr,
		1,
		reg_data,
		len,
		I2C_TRANSMIT_TIMEOUT);

	if (err != ESP_OK) {
		ESP_LOGE(TAG,
				 "I2C read failed: reg=0x%02X len=%" PRIu32
				 " error=%s (0x%x)",
				 reg_addr, len, esp_err_to_name(err),
				 (unsigned int)err);

		return BME280_E_COMM_FAIL;
	}

	return BME280_OK;
}
// }}}

// {{{ bme280_i2c_write()
// Bosch API Callback: I2C Write
int8_t bme280_i2c_write(
	uint8_t reg_addr,
	const uint8_t *reg_data,
	uint32_t len,
	void *intf_ptr)
{
	uint8_t buffer[len + 1];
	buffer[0] = reg_addr;

	for (uint32_t i = 0; i < len; i++) {
		buffer[i + 1] = reg_data[i];
	}

	i2c_master_dev_handle_t dev_handle = *(i2c_master_dev_handle_t *)intf_ptr;

	esp_err_t err = i2c_master_transmit(dev_handle, buffer, len + 1, I2C_TRANSMIT_TIMEOUT);

	if (err != ESP_OK) {
		ESP_LOGE(TAG,
				 "I2C write failed: reg=0x%02X len=%" PRIu32
				 " error=%s (0x%x)",
				 reg_addr, len, esp_err_to_name(err),
				 (unsigned int)err);

		return BME280_E_COMM_FAIL;
	}

	ESP_LOGI(TAG,
	 "I2C write OK: reg=0x%02X len=%" PRIu32
	 " first_data=0x%02X",
	 reg_addr, len, len > 0 ? reg_data[0] : 0);

	return BME280_OK;
}
// }}}

// {{{ bme280_delay_us()
// Bosch API Callback: Delay
void bme280_delay_us(uint32_t period, void *intf_ptr) {
	(void)intf_ptr;
	esp_rom_delay_us(period);
}
// }}}

