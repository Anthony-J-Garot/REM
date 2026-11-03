import sys
from PIL import Image

# Add a scale factor constant near the top or inside your function
SCALE_FACTOR = 4
# I tend to look at the file with Irfanview rather than on the VM.
SHOW_AFTER_SAVE = False

def parse_plane(iter_lines, expected_header, end_marker_str, height=128, width=296):
    # 1. Validate Header
    header = next(iter_lines).strip()
    if header != expected_header:
        raise ValueError(f"Invalid or missing header: expected '{expected_header}', got '{header}'")
    
    pixel_data = bytearray()
    row_count = 0

    # 2. Extract and Validate Rows
    for row_idx in range(height):
        try:
            line = next(iter_lines).strip()
        except StopIteration:
            raise ValueError(f"Unexpected end of data at row {row_idx} for {expected_header}.")

        if line == end_marker_str:
            raise ValueError(f"Premature end marker encountered at row {row_idx} for {expected_header}.")

        if ":" not in line:
            raise ValueError(f"Row {row_idx}: Malformed row (missing separator ':'): {line}")

        checksum_str, hex_data = line.split(":", 1)

        if len(checksum_str) != 2 or len(hex_data) != 74:
            raise ValueError(f"Row {row_idx}: Invalid length.")

        try:
            expected_checksum = int(checksum_str, 16)
            row_bytes = bytes.fromhex(hex_data)
        except ValueError as e:
            raise ValueError(f"Row {row_idx}: Non-hexadecimal data: {e}")

        if len(row_bytes) != 37:
            raise ValueError(f"Row {row_idx}: Decoded byte length is {len(row_bytes)}, expected 37.")

        # 3. Verify XOR Checksum
        calculated_checksum = 0
        for b in row_bytes:
            calculated_checksum ^= b

        if calculated_checksum != expected_checksum:
            raise ValueError(f"Checksum mismatch on Row {row_idx} ({expected_header})! Expected 0x{expected_checksum:02X}, got 0x{calculated_checksum:02X}")

        pixel_data.extend(row_bytes)
        row_count += 1

    # 4. Validate End Marker
    end_marker = next(iter_lines).strip()
    if end_marker != end_marker_str:
        raise ValueError(f"Expected '{end_marker_str}', got: {end_marker}")

    if row_count != height:
        raise ValueError(f"Row count mismatch: processed {row_count}, expected {height}.")

    return pixel_data

def process_framebuffer_lines(lines):
    # Filter out empty lines to make peek operations clean
    cleaned_lines = [l.strip() for l in lines if l.strip()]
    if not cleaned_lines:
        raise ValueError("No data provided.")

    iter_lines = iter(cleaned_lines)
    
    # Peek at the first line to see what plane was provided
    first_line = next(iter_lines, "")
    # Put it back by re-instantiating the iterator
    iter_lines = iter(cleaned_lines)

    width, height = 296, 128

    if first_line.startswith("FRAMEBUFFER_RED"):
        # Only Red plane provided
        print("Parsing Red Plane only...")
        red_bytes = parse_plane(iter_lines, "FRAMEBUFFER_RED 296 128 XOR8 ROW_MAJOR_TOP_DOWN", "END FRAMEBUFFER_RED")
        img = Image.frombytes("1", (width, height), bytes(red_bytes))

        # Scale up preserving aspect ratio
        img = img.resize((width * SCALE_FACTOR, height * SCALE_FACTOR), resample=Image.Resampling.NEAREST)

        output_filename = "epaper_snapshot_red.png"
        img.save(output_filename)
        print(f"Success! Reconstructed red plane snapshot and saved as '{output_filename}'.")
        if SHOW_AFTER_SAVE:
            img.show()
        return

    # Otherwise, assume it starts with the Black plane
    print("Parsing Black Plane...")
    bw_bytes = parse_plane(iter_lines, "FRAMEBUFFER 296 128 XOR8 ROW_MAJOR_TOP_DOWN", "END FRAMEBUFFER")

    # Check if there are more lines remaining for the Red plane
    try:
        next_line = next(iter_lines)
        # Put the remaining lines back into an iterator list
        remaining_lines = [next_line] + list(iter_lines)
    except StopIteration:
        remaining_lines = []

    if remaining_lines and remaining_lines[0].startswith("FRAMEBUFFER_RED"):
        print("Parsing Red Plane...")
        iter_red = iter(remaining_lines)
        red_bytes = parse_plane(iter_red, "FRAMEBUFFER_RED 296 128 XOR8 ROW_MAJOR_TOP_DOWN", "END FRAMEBUFFER_RED")

        # Merge Black and Red planes into RGB
        img_bw = Image.frombytes("1", (width, height), bytes(bw_bytes))
        img_red = Image.frombytes("1", (width, height), bytes(red_bytes))

        final_img = Image.new("RGB", (width, height), (255, 255, 255))
        pixels = final_img.load()
        bw_px = img_bw.load()
        red_px = img_red.load()

        for y in range(height):
            for x in range(width):
                is_black = (bw_px[x, y] == 0)
                is_red = (red_px[x, y] == 0)

                if is_black:
                    pixels[x, y] = (0, 0, 0)
                elif is_red:
                    pixels[x, y] = (220, 20, 60)
                else:
                    pixels[x, y] = (255, 255, 255)

        output_filename = "epaper_snapshot_rgb.png"

        # Scale up the final composite image
        final_img = final_img.resize((width * SCALE_FACTOR, height * SCALE_FACTOR), resample=Image.Resampling.NEAREST)

        final_img.save(output_filename)
        print(f"Success! Reconstructed dual-plane 3-color preview and saved as '{output_filename}'.")
        if SHOW_AFTER_SAVE:
            final_img.show()
    else:
        # Only Black plane was provided
        img = Image.frombytes("1", (width, height), bytes(bw_bytes))

        # Scale up preserving aspect ratio
        img = img.resize((width * SCALE_FACTOR, height * SCALE_FACTOR), resample=Image.Resampling.NEAREST)

        output_filename = "epaper_snapshot.png"
        img.save(output_filename)
        print(f"Success! Reconstructed black-plane snapshot and saved as '{output_filename}'.")
        if SHOW_AFTER_SAVE:
            img.show()

def main():
    print("Reading framebuffer text from stdin (paste output and press Ctrl+D / Ctrl+Z)...")
    lines = sys.stdin.readlines()
    process_framebuffer_lines(lines)

if __name__ == "__main__":
    main()
