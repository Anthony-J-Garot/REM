#!/bin/bash

#
# This makes a tarball for backups. I want everything except the build dir.
#

TODAY=$(date +%Y-%m-%d)
TAR_FILE=~/Music/${TODAY}_REM_backup.tar

# Clean out tests first
(
    cd test
    if ! make clean; then
        echo "Failed to clean test binaries"
        exit 1
    fi
)

echo $PWD

# Make the tarball. Put in Music directory for me to grab a copy during ripping.
# Do not gzip it. It's less than 1MB now, and it's useful to share this with AI sometimes.
tar --exclude='./build' --exclude='./components' --exclude="./_archive" -cf $TAR_FILE .

echo "Wrote to $TAR_FILE"
