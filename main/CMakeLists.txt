# Choose your active layout here
# set(ACTIVE_LAYOUT "display_layout_1.c")
# set(ACTIVE_LAYOUT "display_layout_2.c")
set(ACTIVE_LAYOUT "display_layout_4.c")

idf_component_register(
    SRCS 
		"sensor_bme280.c"		# device
        "display_epaper.c"		# device
        "main.c"
        "i2c_helpers.c"
        "sensor_utils.c"
        "uart_task.c"
		"layouts/${ACTIVE_LAYOUT}"
		#"fonts/font_8x8.c"
		#"fonts/font_8x16.c"

		# Layout 4 splits glyphs into icons and fonts
        "fonts/font_20x32.c"
		"icons/thermometer.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES
        esp_driver_i2c
        esp_driver_gpio
        esp_driver_spi
        bme280
)

# Comment out for release
target_compile_definitions(${COMPONENT_LIB} PRIVATE ENABLE_DEBUG_CONSOLE)

