#include "uart_task.h"

#ifdef ENABLE_DEBUG_CONSOLE

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static const char *TAG = "CONSOLE";

// {{{ console_parse_line()
/**
 * @brief Parses a command string and populates an event.
 * @return true if string represents a valid command, false otherwise.
 *
 * NOTE: I don't have every possible command, just the ones I use a lot.
 */
bool console_parse_line(const char *line) {
	if (!line) { return false; }

	char buf[64];
	strncpy(buf, line, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = '\0';

	char *arg = NULL;
	char *cmd = strtok_r(buf, " ", &arg);
	if (!cmd) { return false; }

	if (strcmp(cmd, "refresh") == 0) {
		ESP_LOGI(TAG, "Executing refresh request command...");

		// Trigger the e-paper refresh request
		request_epaper_refresh();

		return true;
	}

	if (strcmp(cmd, "override") == 0) {
		ESP_LOGI(TAG, "Executing refresh override command...");

		// Trigger the e-paper override
		override_epaper_refresh();

		return true;
	}

	ESP_LOGI(TAG, "Unknown command: %s", line);
	return false;
}
// }}}

// {{{ console_process_char()
/**
 * @brief Processes an incoming keystroke into a line buffer with echoing and editing.
 * @return true when a complete command line is submitted (ENTER pressed), false otherwise.
 */
bool console_process_char(int keystroke, char *buffer, size_t max_len, size_t *inout_idx) {
	if (keystroke == EOF) { return false; }

	size_t idx = *inout_idx;

	// Process ENTER key
	if (keystroke == '\r' || keystroke == '\n') {
		putchar('\n');
		fflush(stdout);

		if (idx > 0) {
			buffer[idx] = '\0'; // Null-terminate command string
			*inout_idx = 0;	 // Reset buffer index
			return true;
		}
		return false;
	}

	// Process BACKSPACE / DELETE
	if ((keystroke == '\b' || keystroke == 0x7F) && idx > 0) {
		idx--;
		printf("\b \b");
		fflush(stdout);
		*inout_idx = idx;
		return false;
	}

	// Process standard printable characters
	if (idx < max_len - 1 && keystroke >= 32 && keystroke <= 126) {
		buffer[idx++] = (char)keystroke;
		putchar(keystroke);
		fflush(stdout);
		*inout_idx = idx;
	}

	return false;
}
// }}}

// {{{ uart_task()
// This is natively non-blocking, so I'll use getchar() and tack onto a string.
void uart_task(void *pvParameters) {
	char buffer[64];
	size_t idx = 0;

	ESP_LOGV(TAG, "uart_task(): entered...");

	while (1) {
		int keystroke = getchar();

		if (console_process_char(keystroke, buffer, sizeof(buffer), &idx)) {
			// Watch for safe stack usage
			CHECK_STACK_HEADROOM(TAG, 512);

			if (! console_parse_line(buffer)) {
				ESP_LOGE(TAG, "Failed to parse line");
			}
		}

		vTaskDelay(pdMS_TO_TICKS(50)); // Sleep 50ms to yield CPU when idle
	}
}
// }}}

#endif

