#pragma once

#include "display_draw.h"
#include "esp_err.h"

// Initialize GPIOs and SPI bus
esp_err_t epaper_init(void);

// Hardware control routines
void epaper_reset(void);
void epaper_wait_until_idle(void);

// Framebuffer management
void epaper_clear_framebuffers(epaper_color_t background);

// Refresh routine to push buffers to the panel
void epaper_display_refresh(void);

