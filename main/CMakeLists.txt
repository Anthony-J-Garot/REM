# Choose your active layout here
# set(ACTIVE_LAYOUT "display_layout_1.c")
# set(ACTIVE_LAYOUT "display_layout_2.c")
set(ACTIVE_LAYOUT "display_layout_4.c")

# Dumps a visual display instead of rendering to the physical epaper module.
# Set to 0 to use the regular display.
set(ENABLE_EPAPER_UART_EXPORT "0")

# The Debug console is useful for sending commands. Set to 0 for release.
set(ENABLE_DEBUG_CONSOLE "0")

idf_component_register(
    SRCS 
		"sensor_bme280.c"		# device
        "display_epaper.c"		# device
        "display_draw.c"
        "display_export.c"
        "main.c"
        "i2c_helpers.c"
        "sensor_utils.c"
        "uart_task.c"
		"layouts/${ACTIVE_LAYOUT}"

		# Additional available fonts not used in current layout.
		#"fonts/font_8x8.c"
		#"fonts/font_8x16.c"

		# Layout 4 
		# This layout splits glyphs into icons and fonts.
        "fonts/font_20x32.c"
		"icons/thermometer.c"
		"icons/waterdrop.c"
		"icons/dial.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES
        esp_driver_i2c
        esp_driver_gpio
        esp_driver_spi
        bme280
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE 
	ENABLE_DEBUG_CONSOLE=${ENABLE_DEBUG_CONSOLE}
    ENABLE_EPAPER_UART_EXPORT=${ENABLE_EPAPER_UART_EXPORT}
)

