#include <stdio.h>
#include <inttypes.h>
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "uart_task.h"
#include "i2c_helpers.h"
#include "display_epaper.h"
#include "sensor_bme280.h"
#include "sensor_utils.h"
#include "glyph.h"
#include "display_formatter.h"
#if defined(ENABLE_EPAPER_UART_EXPORT) && (ENABLE_EPAPER_UART_EXPORT == 1)
	#include "display_export.h"
#endif

static const char *TAG = "main";

/*
 * The manufacturer recommends 180 seconds minimum between display refreshes.
 * Don't recompile & flash too quickly. 180 seconds is 180 seconds. You 
 * don't want to flash while the display is refreshing, either.
 */
static const int MFG_MIN_REFRESH_INTERVAL = 180; // seconds; based upon board.

// {{{ refresh_display()
static void refresh_display(float temp_f, float pressure_inhg, float humidity, float dew_pt_h) {
	ESP_LOGI(TAG, "Refreshing e-paper display from console trigger...");

	// Clear buffer to white
	epaper_clear_framebuffers(EPAPER_COLOR_WHITE);

	// Draw the whole layout
	display_formatter_draw(temp_f, pressure_inhg, humidity, dew_pt_h);

#if defined(ENABLE_EPAPER_UART_EXPORT) && (ENABLE_EPAPER_UART_EXPORT == 1)
	// Export over UART without touching the physical e-paper panel
	epaper_export_bw_framebuffer_uart();	// Export Black Plane
	epaper_export_red_framebuffer_uart();	// Export Red Plane right after
#else
	// Normal refresh pushes updates to hardware via SPI
	epaper_display_refresh();
	ESP_LOGI(TAG, "E-paper refresh complete.");
#endif
}

// }}}

// Task handle for app_main so uart_task can send notifications to it
static TaskHandle_t app_main_task_handle = NULL;

// {{{ ENABLE_DEBUG_CONSOLE
#if defined(ENABLE_DEBUG_CONSOLE) && (ENABLE_DEBUG_CONSOLE == 1)
void request_epaper_refresh(void) {
	if (app_main_task_handle != NULL) {
		xTaskNotify(app_main_task_handle, CMD_REFRESH_REQUEST, eSetValueWithOverwrite);
	}
}

void override_epaper_refresh(void) {
	if (app_main_task_handle != NULL) {
		xTaskNotify(app_main_task_handle, CMD_REFRESH_OVERRIDE, eSetValueWithOverwrite);
	}
}

// {{{ handle_epaper_notifications()
static void handle_epaper_notifications(
	BaseType_t notified, 
	uint32_t notification_value, 
	int *refresh_cooldown, 
	float temp_f, 
	float pressure_inhg, 
	float humidity, 
	float dew_pt_h) {

	if (notified != pdTRUE) { return; }

	bool allow_refresh = false;

	if (notification_value == CMD_REFRESH_OVERRIDE) {
		// This is a hard request that bypasses the cooldown check. 
		// Do this when you are certain the manufacturing minimum has been met.
		ESP_LOGW(TAG, "Refresh override: bypassing cooldown.");
		allow_refresh = true; 
	} 
	else if (notification_value == CMD_REFRESH_REQUEST) {
		// This is a soft request. It waits the manufacturer's limit.
		ESP_LOGI(TAG, "Refresh request command received.");
		if (*refresh_cooldown == 0) {
			allow_refresh = true;
		} else {
			ESP_LOGW(
				TAG, 
				"Refresh denied: Manufacturer minimum %ds not met. %d seconds remaining.", 
				MFG_MIN_REFRESH_INTERVAL, 
				*refresh_cooldown);
		}
	}

	// Execute refresh if allowed
	if (allow_refresh) {
		refresh_display(temp_f, pressure_inhg, humidity, dew_pt_h);
		*refresh_cooldown = MFG_MIN_REFRESH_INTERVAL; // Reset cooldown timer
	}
}
// }}}

#endif
// }}}

// {{{ app_main()
void app_main(void)
{
	// Capture the current task handle (app_main)
	app_main_task_handle = xTaskGetCurrentTaskHandle();

	// Initialize ESP-IDF I2C Master Bus
	i2c_master_bus_config_t bus_config = {
		.i2c_port = I2C_MASTER_NUM,
		.sda_io_num = I2C_MASTER_SDA_IO,
		.scl_io_num = I2C_MASTER_SCL_IO,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.flags.enable_internal_pullup = false,
	};

	i2c_master_bus_handle_t bus_handle;
	ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

#if defined(ENABLE_DEBUG_CONSOLE) && (ENABLE_DEBUG_CONSOLE == 1)
	// I2C scanner
	int devices_found = i2c_scanner_run(bus_handle);

	// Assumes only bme280 device is connected to I2C
	if (devices_found == 0) {
		ESP_LOGE(TAG, "No I2C devices found. Aborting initialization.");
		return;
	}
#endif

	// Initialize BME280 Environmental Sensor
	// ... (bus configuration and scanner code) ...
	ESP_ERROR_CHECK(sensor_bme280_init(bus_handle));

	// Initialize e-paper display SPI & control lines
	ESP_ERROR_CHECK(epaper_init());
	epaper_reset();
	epaper_wait_until_idle(); // Ensure panel is idle post-reset
	ESP_LOGI(TAG, "E-paper hardware reset sent and idle check passed.");

	// {{{ Spawn uart_task
#if defined(ENABLE_DEBUG_CONSOLE) && (ENABLE_DEBUG_CONSOLE == 1)
	BaseType_t ret = xTaskCreate(
		uart_task,
		"uart_task",
		3072,
		NULL,
		1,
		NULL
	);
	if (ret != pdPASS) {
		ESP_LOGE(TAG, "Failed to create uart_task");
		return;
	}
#endif
	// }}}

	// Periodic Measurement Loop
	struct bme280_data bme_data;
	//int refresh_delay = -MFG_MIN_REFRESH_INTERVAL; // Wait a full 180 seconds on startup/reflash.
	int refresh_cooldown = MFG_MIN_REFRESH_INTERVAL; // Start with a full 180s wait on boot
	while (1) {
		uint32_t notification_value = 0;

		// Wait up to 1000ms (1 second) for a notification from uart_task.
		// This replaces vTaskDelay() and wakes up instantly if a command is sent.
		BaseType_t notified = xTaskNotifyWait(
			0x00,					// Don't clear bits on entry
			0xFFFFFFFF,				// Clear all bits on exit
			&notification_value,	// Receive the command enum value
			pdMS_TO_TICKS(1000)		// 1-second timeout
		);

		// Decrement cooldown timer if active
		if (refresh_cooldown > 0) {
			refresh_cooldown--;
			if (refresh_cooldown == 0) {
				ESP_LOGI(TAG, "E-paper refresh cooldown expired. Refresh is now allowed.");
			}
		}

		if (sensor_bme280_get_data(&bme_data) != BME280_OK) {
			ESP_LOGE(TAG, "Failed to stream sensor data");
			continue;
		}

		// Conversions
		float temp_f = c_to_f((float)bme_data.temperature);
		float pressure_inhg = hpa_to_inhg((float) bme_data.pressure / 100.0F);
		float dew_pt_h = c_to_f(calculate_dew_point((float) bme_data.temperature, (float) bme_data.humidity));

#if defined(ENABLE_EPAPER_UART_EXPORT) && (ENABLE_EPAPER_UART_EXPORT == 1)
		refresh_display(temp_f, pressure_inhg, (float) bme_data.humidity, dew_pt_h);
		ESP_LOGI(TAG, "Not resuming execution after EXPORT.");
		vTaskSuspend(NULL);
#endif

		// Log to serial frequently for immediate diagnostics
		ESP_LOGI(TAG, 
			"Cooldown: %ds Temp: %.2f °C (%.2f °F) | Press: %.2f hPa (%.2f inHg) | Humidity: %.2f %% | Dew Point: %.2f °F", 
			refresh_cooldown,
			bme_data.temperature, 
			temp_f, 
			(bme_data.pressure / 100.0F), 
			pressure_inhg, 
			bme_data.humidity, 
			dew_pt_h);

#if defined(ENABLE_DEBUG_CONSOLE) && (ENABLE_DEBUG_CONSOLE == 1)
		// Handle incoming console commands if a notification was received
		handle_epaper_notifications(
			notified, 
			notification_value, 
			&refresh_cooldown, 
			temp_f, 
			pressure_inhg, 
			(float) bme_data.humidity, 
			dew_pt_h);
#else
		(void) notified;

		if (refresh_cooldown == 0) {
			refresh_display(temp_f, pressure_inhg, (float) bme_data.humidity, dew_pt_h);
			refresh_cooldown = MFG_MIN_REFRESH_INTERVAL; // Reset cooldown timer
		}
#endif

	}
}
// }}}

