# REM — Room Environmental Monitor

A standalone, low-power environmental monitoring project built around the ESP32-H2, Bosch BME280 sensor, and a 2.9-inch three-color e-paper display.

REM measures and displays temperature, relative humidity, atmospheric pressure, and calculated dew point. It is designed as a self-contained embedded device, with an emphasis on reliable operation, efficient hardware utilization, and clear presentation of environmental data.

## Project Overview

REM is an embedded engineering project developed using native ESP-IDF and C. It combines environmental sensing, sensor data processing, custom display rendering, and low-level hardware integration into a standalone device.

The project explores many important facets of an embedded engineering project, including:

- Microcontroller programming using native ESP-IDF.
- Modular embedded C architecture.
- I²C communication and environmental sensor integration.
- SPI communication and e-paper display control.
- Custom bitmap fonts and graphics rendering.
- Temperature, humidity, pressure, and dew point calculations.
- Hardware initialization, error handling, and diagnostic output.
- Unit testing of hardware-independent functionality.
- Hardware documentation and reproducible build configuration.

The goal is not simply to display sensor readings, but to develop a complete, understandable, and reproducible embedded system.

## Hardware

The current implementation uses the following components:

| Component | Description |
|---|---|
| Microcontroller | ESP32-H2 Super Mini |
| Environmental sensor | Bosch BME280 |
| Display | 2.9-inch, 296 × 128, three-color e-paper |
| Display controller | SSD1680 |
| Development framework | ESP-IDF |

The ESP32-H2 provides the processing and peripheral interfaces required by the project. The BME280 communicates over I²C, while the e-paper display uses SPI.

### Hardware Interfaces

**I²C — BME280**

- SDA: GPIO11
- SCL: GPIO10
- Bus frequency: 100 kHz

**SPI — E-paper display**

- SCLK: GPIO4
- MOSI: GPIO5
- CS: GPIO14
- D/C: GPIO13
- RESET: GPIO12
- BUSY: GPIO23

Additional GPIO assignments include:

- Status LED: GPIO13 (shared with the display D/C assignment as applicable to the hardware configuration).
- Button: GPIO0
- Auxiliary input: GPIO12
- Auxiliary output: GPIO14

*Note: Pin assignments and peripheral sharing will be verified and finalized in the hardware documentation.*

## Software Architecture

The firmware is organized into separate modules to keep hardware interfaces, sensor acquisition, calculations, and display rendering independent where practical.

The architecture is intended to make individual components easier to understand, test, maintain, and potentially reuse.

Key areas include:

- Sensor acquisition and environmental measurements.
- Temperature conversion and unit handling.
- Atmospheric pressure conversion.
- Dew point calculation.
- E-paper initialization and refresh management.
- Bitmap font and custom glyph rendering.
- Main application control and diagnostics.

The display implementation includes custom graphics and bitmap fonts rather than relying exclusively on a conventional text-rendering library.

## Display

REM uses a 2.9-inch, 296 × 128-pixel, black/white/red e-paper display.

The display driver handles initialization, SPI communication, framebuffer management, and refresh operations.

Custom rendering routines support:

- Bitmap fonts.
- Custom-sized graphical glyphs.
- Multiple display colors.
- Pixel-level drawing.
- Text and graphic positioning.

The display is designed around readability and efficient use of the available resolution.

[Insert photograph of completed display]

## Environmental Measurements

The BME280 provides the primary environmental measurements:

- **Temperature:** Ambient temperature.
- **Relative humidity:** Percentage of relative humidity.
- **Atmospheric pressure:** Barometric pressure.
- **Dew point:** Calculated from temperature and relative humidity.

Pressure and temperature values can be converted into commonly used engineering units.

Dew point is calculated in firmware rather than requiring an additional sensor.

## Development Environment

REM is developed using:

- ESP-IDF 6.0.2
- Embedded C
- CMake-based ESP-IDF build system
- ESP32-H2 toolchain

The project uses the native ESP-IDF framework rather than Arduino abstractions.

## Building and Flashing

[To be completed]

This section will include:

- ESP-IDF installation and configuration.
- Required components and dependencies.
- Project configuration.
- Build commands.
- Flashing instructions.
- Serial monitor configuration.

## Testing

Hardware-independent functionality is being separated from peripheral-dependent code to facilitate unit testing.

Planned test coverage includes:

- Environmental calculations.
- Temperature and pressure conversions.
- Dew point calculations.
- Data validation and boundary conditions.
- Other hardware-independent utility functions.

[Update this section with actual test coverage and execution instructions before release.]

## Project Scope

REM is intended to operate as a standalone environmental monitor.

The initial release focuses on local sensing, processing, and display functionality. Wireless communication, remote monitoring, and integration with external home-automation systems are outside the scope of the initial release.

The hardware and firmware architecture leave room for future experimentation without making those features dependencies of the core application.

## License

This project is released under the MIT License.

See [LICENSE](LICENSE) for details.

You are free to use, modify, distribute, and incorporate this software into other projects, subject to the terms of the license.

## Acknowledgments

- Espressif Systems for ESP-IDF and the ESP32-H2 platform.
- Bosch Sensortec for the BME280 sensor and associated documentation.
- WeAct Studio for the e-paper display hardware.