#!/bin/bash

PYTHON="$(which python3)"

# Get the script directory. This allows you to call this script from anywhere.
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

{
	cd "$DIR"

	$PYTHON epaper_decode.py < capture.txt
}

