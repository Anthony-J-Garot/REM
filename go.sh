#!/usr/bin/env bash
set -e

export REM_PORT="/dev/serial/by-id/usb-Espressif_USB_JTAG_serial_debug_unit_74:4D:BD:6E:71:90-if00"
IDF_PATH="${IDF_PATH:-/storage/.espressif/v6.0.2/esp-idf}"
FLASH_BAUD=921600
MONITOR_BAUD=115200

# Ensure the path
if [ ! -d "$IDF_PATH" ]; then
    echo "Could not find $IDF_PATH directory"
    exit 1
fi

# Initialize ESP-IDF if necessary.
if ! command -v idf.py >/dev/null 2>&1; then
    if ! source "$IDF_PATH/export.sh" >/dev/null 2>&1; then
        echo "idf.py is still unavailable after sourcing export.sh."
        exit 1
    fi
fi

# Verify idf.py is now available.
if ! command -v idf.py >/dev/null 2>&1; then
    echo "Unable to initialize the ESP-IDF environment."
    echo
    echo "If this is a new installation or tools are missing, run:"
    echo
    echo "    $IDF_PATH/install.sh esp32"
    exit 1
fi

# Ensure the serial port is set
if [ ! -e "$REM_PORT" ]; then
    echo "REM serial device not found: $REM_PORT"
    exit 1
fi

# If arguments are passed in, run idf.py with all of them.
# Examples:
#   reconfig
#   menuconfig
#   fullclean
#   monitor --output-file esp32_log.txt
#   monitor --no-reset
if [[ $# -gt 0 ]]; then
    idf.py -p "$REM_PORT" "$@"
    exit 0
fi

# If dropped through (normal mode), run everything.

echo "🚀 [FAST BUILD] Target: Application Only..."
idf.py app

echo "🚀 [FAST FLASH] Target: Application Only..."
idf.py -p "$REM_PORT" -b "$FLASH_BAUD" app-flash

echo "Monitoring..."
exec idf.py -p "$REM_PORT" -b "$MONITOR_BAUD" monitor

