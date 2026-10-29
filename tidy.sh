#!/bin/bash
set -e

BUILD_DIR="build"
COMPILE_COMMANDS="${BUILD_DIR}/compile_commands.json"
CODE_FILES=(main/*.c)

# Extract only project-local include paths, filtering out raw toolchain sysroots
EXTRA_INCLUDES=""
if [ -f "$COMPILE_COMMANDS" ]; then
    EXTRA_INCLUDES=$(python3 -c '
import json, re
with open("'$COMPILE_COMMANDS'", "r") as f:
    data = json.load(f)
includes = set()
for entry in data:
    cmd = entry.get("command", "") + " " + " ".join(entry.get("arguments", []))
    found = re.findall(r"-I\S+", cmd)
    for inc in found:
        clean_inc = inc.strip("\x27\x22")
        if "sysroot" not in clean_inc and "riscv32-unknown-elf/include" not in clean_inc:
            includes.add(clean_inc)
print(" ".join(includes))
    ')
fi

# Run clang-tidy, forcing host target, standard flags, project includes, 
# and adding a preprocessor guard macro to bypass missing newlib/sys headers if needed
clang-tidy -p "$BUILD_DIR" "${CODE_FILES[@]}" -- \
    -std=gnu11 \
    -target x86_64-pc-linux-gnu \
    -fsyntax-only \
    -D__STDC_NO_ATOMICS__ \
    $EXTRA_INCLUDES
