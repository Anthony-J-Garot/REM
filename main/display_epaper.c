#include "display_epaper.h"
#include <string.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

static const char *TAG = "epaper";

#define EPIN_SCLK	4
#define EPIN_MOSI	5
#define EPIN_CS		14
#define EPIN_DC		13
#define EPIN_RST	12
#define EPIN_BUSY	23

// SSD1680 RAM layout for the 296 x 128 landscape framebuffer.
// The controller's native layout is 16 bytes x 296 rows.
#define EPAPER_RAM_X_START	0x00
#define EPAPER_RAM_X_END	0x0F
#define EPAPER_RAM_Y_START	0x0000
#define EPAPER_RAM_Y_END	0x0127	// 295

static spi_device_handle_t spi_handle = NULL;

// Dual framebuffers for Black/White and Red planes
static uint8_t bw_buffer[EPAPER_BUFFER_SIZE];
static uint8_t red_buffer[EPAPER_BUFFER_SIZE];

// {{{ epaper_send_command()
static esp_err_t epaper_send_command(uint8_t cmd)
{
	gpio_set_level(EPIN_DC, 0);

	spi_transaction_t transaction = {
		.length = 8,
		.tx_buffer = &cmd,
	};

	return spi_device_polling_transmit(spi_handle, &transaction);
}
// }}}

// {{{ epaper_send_data()
static esp_err_t epaper_send_data(const uint8_t *data, size_t length)
{
	if (length == 0) { return ESP_OK; }

	gpio_set_level(EPIN_DC, 1);

	spi_transaction_t transaction = {
		.length = length * 8,
		.tx_buffer = data,
	};

	return spi_device_polling_transmit(spi_handle, &transaction);
}
// }}}

// {{{ epaper_send_command_data()
static esp_err_t epaper_send_command_data(
	uint8_t cmd,
	const uint8_t *data,
	size_t length)
{
	esp_err_t err = epaper_send_command(cmd);
	if (err != ESP_OK) { return err; }

	return epaper_send_data(data, length);
}
// }}}

// {{{ epaper_set_ram_window()
static esp_err_t epaper_set_ram_window(void)
{
	const uint8_t entry_mode = 0x03; // X increments, then Y
	const uint8_t x_window[] = {
		EPAPER_RAM_X_START,
		EPAPER_RAM_X_END
	};
	const uint8_t y_window[] = {
		(uint8_t)(EPAPER_RAM_Y_START & 0xFF),
		(uint8_t)(EPAPER_RAM_Y_START >> 8),
		(uint8_t)(EPAPER_RAM_Y_END & 0xFF),
		(uint8_t)(EPAPER_RAM_Y_END >> 8)
	};
	const uint8_t x_counter = EPAPER_RAM_X_START;
	const uint8_t y_counter[] = {
		(uint8_t)(EPAPER_RAM_Y_START & 0xFF),
		(uint8_t)(EPAPER_RAM_Y_START >> 8)
	};

	esp_err_t err;

	err = epaper_send_command_data(0x11, &entry_mode, 1);
	if (err != ESP_OK) { return err; }

	err = epaper_send_command_data(0x44, x_window, sizeof(x_window));
	if (err != ESP_OK) { return err; }

	err = epaper_send_command_data(0x45, y_window, sizeof(y_window));
	if (err != ESP_OK) { return err; }

	err = epaper_send_command_data(0x4E, &x_counter, 1);
	if (err != ESP_OK) { return err; }

	err = epaper_send_command_data(0x4F, y_counter, sizeof(y_counter));
	if (err != ESP_OK) { return err; }

	return ESP_OK;
}
// }}}

// {{{ epaper_init()
esp_err_t epaper_init(void)
{
	gpio_config_t io_conf = {
		.pin_bit_mask = (1ULL << EPIN_DC) | (1ULL << EPIN_RST),
		.mode = GPIO_MODE_OUTPUT,
	};
	gpio_config(&io_conf);

	gpio_config_t busy_conf = {
		.pin_bit_mask = (1ULL << EPIN_BUSY),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
	};
	gpio_config(&busy_conf);

	spi_bus_config_t buscfg = {
		.sclk_io_num = EPIN_SCLK,
		.mosi_io_num = EPIN_MOSI,
		.miso_io_num = -1,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 5000,
	};
	
	esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
	if (ret != ESP_OK) { return ret; }

	spi_device_interface_config_t devcfg = {
		.clock_speed_hz = 4 * 1000 * 1000,
		.mode = 0,
		.spics_io_num = EPIN_CS,
		.queue_size = 7,
	};
	return spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);
}
// }}}

// {{{ epaper_reset()
void epaper_reset(void)
{
	gpio_set_level(EPIN_RST, 0);
	vTaskDelay(pdMS_TO_TICKS(20));
	gpio_set_level(EPIN_RST, 1);
	vTaskDelay(pdMS_TO_TICKS(20));
}
// }}}

// {{{ epaper_wait_until_idle()
void epaper_wait_until_idle(void)
{
	ESP_LOGI(TAG, "Waiting for e-paper to become idle (BWR panels take ~19s)...");
	
	// 19 seconds refresh time means we need a longer timeout (~25 seconds safety limit)
	int timeout_counter = 500; // 500 * 50ms = 25,000ms (25 seconds)
	
	while (gpio_get_level(EPIN_BUSY) == 1) {
		vTaskDelay(pdMS_TO_TICKS(50));
		timeout_counter--;
		if (timeout_counter <= 0) {
			ESP_LOGW(TAG, "E-paper busy timeout! Continuing anyway.");
			break;
		}
	}
	
	// Give the panel a stable pause after busy goes low
	vTaskDelay(pdMS_TO_TICKS(100));
	ESP_LOGI(TAG, "E-paper is now idle.");
}
// }}}

// {{{ epaper_clear_framebuffers()
void epaper_clear_framebuffers(epaper_color_t background)
{
	// Black plane: 0 = black, 1 = white or red
	memset(
		bw_buffer,
		(background == EPAPER_COLOR_BLACK) ? 0x00 : 0xFF,
		sizeof(bw_buffer));

	// Red plane before transmission inversion:
	// 0 = red, 1 = white or black
	memset(
		red_buffer,
		(background == EPAPER_COLOR_RED) ? 0x00 : 0xFF,
		sizeof(red_buffer));
}
// }}}

// {{{ epaper_draw_pixel()
void epaper_draw_pixel(int pixel_x, int pixel_y, epaper_color_t color)
{
	// Check bounds for landscape orientation (296 x 128)
	if (pixel_x < 0 || pixel_x >= EPAPER_WIDTH || pixel_y < 0 || pixel_y >= EPAPER_HEIGHT) { return; }

	// Transpose coordinates: map landscape (x, y) to the panel's native portrait/scanned memory layout
	// For many WeAct 2.9" panels, native RAM expects Y as the byte column and X as the bit/row stride.
	int rotated_x = pixel_y;
	int rotated_y = EPAPER_WIDTH - 1 - pixel_x;

	int addr = (rotated_x / 8) + (rotated_y * (EPAPER_HEIGHT / 8));
	int bit = 7 - (rotated_x % 8);

	if (addr < 0 || addr >= EPAPER_BUFFER_SIZE) { return; }

	if (color == EPAPER_COLOR_BLACK) {
		bw_buffer[addr] &= ~(1 << bit);
		red_buffer[addr] |= (1 << bit); 
	} else if (color == EPAPER_COLOR_RED) {
		bw_buffer[addr] |= (1 << bit);
		red_buffer[addr] &= ~(1 << bit); 
	} else { // White
		bw_buffer[addr] |= (1 << bit);
		red_buffer[addr] |= (1 << bit);
	}
}
// }}}

// {{{ epaper_draw_string()
void epaper_draw_string(int pixel_x, int pixel_y, const char *text, const epaper_font_t *font, epaper_color_t color)
{
	if (!font || !font->table) { return; }

	int bytes_per_row = (font->width + 7) / 8;

	while (*text) {
		char chr = *text++;
		int font_idx = chr - font->start_char;
		
		if (font_idx < 0 || font_idx > (font->end_char - font->start_char)) {
			font_idx = 0; // Fallback for out-of-bounds characters
		}

		// Calculate pointer to the specific character glyph bytes
		const uint8_t *glyph = font->table + ((ptrdiff_t)font_idx * font->height * bytes_per_row);

		for (int i = 0; i < font->height; i++) {
			for (int j = 0; j < font->width; j++) {
				int byte_idx = j / 8;
				int bit_idx = 7 - (j % 8);
				
				if (glyph[(i * bytes_per_row) + byte_idx] & (1 << bit_idx)) {
					epaper_draw_pixel(pixel_x + j, pixel_y + i, color);
				}
			}
		}
		pixel_x += font->width; // Advance character width dynamically
	}
}
// }}}

// {{{ epaper_draw_glyph()
// A glyph may be useful for unicode images.
void epaper_draw_glyph(int pixel_x, int pixel_y, const epaper_glyph_t *glyph, epaper_color_t color)
{
	if (!glyph || !glyph->table) { return; }

	int bytes_per_row = (glyph->width + 7) / 8;

	const uint8_t *data = glyph->table;

	for (int i = 0; i < glyph->height; i++) {
		for (int j = 0; j < glyph->width; j++) {
			int byte_idx = j / 8;
			int bit_idx = 7 - (j % 8);
			
			if (data[(i * bytes_per_row) + byte_idx] & (1 << bit_idx)) {
				epaper_draw_pixel(pixel_x + j, pixel_y + i, color);
			}
		}
	}
}
// }}}

// {{{ epaper_draw_box()
void epaper_draw_box(int pixel_x, int pixel_y, int width, int height, epaper_color_t color, bool filled)
{
	for (int py = pixel_y; py < (pixel_y + height); py++) {
		for (int px = pixel_x; px < (pixel_x + width); px++) {
			if (filled || px == pixel_x || px == (pixel_x + width - 1) || py == pixel_y || py == (pixel_y + height - 1)) {
				epaper_draw_pixel(px, py, color);
			}
		}
	}
}
// }}}

// {{{ epaper_display_refresh()
void epaper_display_refresh(void)
{
	const uint8_t driver_output[] = {
		0x27, 0x01, 0x00
	};
	const uint8_t border_waveform = 0x05;
	const uint8_t temperature_sensor = 0x80;
	const uint8_t display_update_control[] = {
		0x00, 0x80
	};
	const uint8_t refresh_control = 0xF7;

	ESP_LOGI(TAG, "Initializing SSD1680 for full refresh");

	// Hardware reset
	epaper_reset();

	// Software reset
	ESP_ERROR_CHECK(epaper_send_command(0x12));
	//epaper_wait_until_idle();
	vTaskDelay(pdMS_TO_TICKS(10));

	// Panel configuration: 296 gate rows
	epaper_send_command_data(0x01, driver_output, sizeof(driver_output));

	// Panel border waveform
	epaper_send_command_data(0x3C, &border_waveform, 1);

	// Internal temperature sensor
	epaper_send_command_data(0x18, &temperature_sensor, 1);

	// Display update control
	epaper_send_command_data(
		0x21,
		display_update_control,
		sizeof(display_update_control));


	// Write black/white plane
	ESP_ERROR_CHECK(epaper_set_ram_window());
	ESP_ERROR_CHECK(epaper_send_command(0x24));
	ESP_ERROR_CHECK(epaper_send_data(bw_buffer, sizeof(bw_buffer)));

	// Write red plane with inverted polarity without modifying framebuffer
	epaper_set_ram_window();
	epaper_send_command(0x26);

	for (size_t i = 0; i < sizeof(red_buffer); i++) {
		uint8_t inverted_byte = (uint8_t)~red_buffer[i];
		epaper_send_data(&inverted_byte, 1);
	}

	// Request a full display refresh
	epaper_send_command_data(0x22, &refresh_control, 1);
	ESP_ERROR_CHECK(epaper_send_command(0x20));

	// Do not start another operation until refresh completes
	epaper_wait_until_idle();

	ESP_LOGI(TAG, "SSD1680 refresh sequence completed");
}
// }}}

