/*
Routines that handle keystrokes from the USB monitor.
The main code is encapsulated in #ifdef ENABLE_DEBUG_CONSOLE rails
to compile out during release.

In menuconfig: Component config > ESP-STDIO > Channel for console output

Choose (X) USB Serial/JTAG Controller

Verify:
grep -E 'CONFIG_ESP_CONSOLE_(USB_SERIAL_JTAG|UART_DEFAULT|UART_NUM)' sdkconfig
*/

#pragma once

#include <stddef.h>
#include "esp_log.h"

#ifdef ENABLE_DEBUG_CONSOLE
    #define CHECK_STACK_HEADROOM(tag, min_margin_bytes) do { \
        unsigned int unused = (unsigned int)uxTaskGetStackHighWaterMark(NULL); \
        if (unused < (min_margin_bytes)) { \
            ESP_LOGE((tag), "STACK WARNING: Only %u bytes left on task '%s'!", \
                    unused, pcTaskGetName(NULL)); \
        } \
    } while(0)
#else
    #define CHECK_STACK_HEADROOM(tag, min_margin_bytes) ((void)0)
#endif

// Define event/command enum
typedef enum {
    CMD_REFRESH_REQUEST,
    CMD_REFRESH_OVERRIDE
} epaper_cmd_t;

/**
 * @brief FreeRTOS task function for interactive console input.
 */
extern void uart_task(void *pvParameters);

extern bool console_parse_line(const char *line);
extern bool console_process_char(int keystroke, char *buffer, size_t max_len, size_t *inout_idx);

extern void request_epaper_refresh(void);
extern void override_epaper_refresh(void);

