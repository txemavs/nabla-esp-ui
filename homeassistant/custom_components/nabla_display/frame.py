# Frame decode utilities for Nabla Display mirror.
# Converts raw RGB332/mono1 bytes from ESP devices to PIL images.
# See docs/platform/DISPLAY-MIRROR-CONTRACT.md for byte layout spec.

from io import BytesIO

from PIL import Image


def rgb332_to_rgb888(byte: int) -> tuple[int, int, int]:
    """Convert RGB332 byte to RGB888 tuple.

    Byte layout: RRRGGGBB (3 bits red, 3 bits green, 2 bits blue).
    """
    r = ((byte >> 5) & 0x07) * 255 // 7
    g = ((byte >> 2) & 0x07) * 255 // 7
    b = (byte & 0x03) * 255 // 3
    return (r, g, b)


def decode_rgb332(data: bytes, width: int, height: int) -> Image.Image:
    """Decode RGB332 frame to PIL RGB image.

    Args:
        data: Raw frame bytes (width * height bytes).
        width: Display width in pixels.
        height: Display height in pixels.

    Returns:
        PIL Image in RGB mode.
    """
    expected_size = width * height
    if len(data) != expected_size:
        raise ValueError(f"Frame size mismatch: expected {expected_size}, got {len(data)}")

    pixels = bytearray(width * height * 3)
    for i, byte in enumerate(data):
        r, g, b = rgb332_to_rgb888(byte)
        pixels[i * 3] = r
        pixels[i * 3 + 1] = g
        pixels[i * 3 + 2] = b

    image = Image.frombytes("RGB", (width, height), bytes(pixels))
    return image


def decode_mono1(data: bytes, width: int, height: int) -> Image.Image:
    """Decode mono1 MSB-first frame to PIL grayscale image.

    Byte layout: MSB first, row-major. Bit 7 of byte 0 is pixel 0.
    White pixel = bit set (1); black pixel = bit clear (0).

    Args:
        data: Raw frame bytes (ceil(width * height / 8) bytes).
        width: Display width in pixels.
        height: Display height in pixels.

    Returns:
        PIL Image in L (grayscale) mode.
    """
    total_pixels = width * height
    expected_size = (total_pixels + 7) // 8
    if len(data) != expected_size:
        raise ValueError(f"Frame size mismatch: expected {expected_size}, got {len(data)}")

    pixels = bytearray(total_pixels)
    for i in range(total_pixels):
        byte_idx = i // 8
        bit_idx = 7 - (i % 8)
        pixels[i] = 255 if (data[byte_idx] >> bit_idx) & 1 else 0

    image = Image.frombytes("L", (width, height), bytes(pixels))
    return image


def decode_frame(data: bytes, width: int, height: int, format_: str) -> Image.Image:
    """Decode frame based on format string.

    Args:
        data: Raw frame bytes.
        width: Display width in pixels.
        height: Display height in pixels.
        format_: Pixel format ("rgb332" or "mono1").

    Returns:
        PIL Image.
    """
    if format_ == "rgb332":
        return decode_rgb332(data, width, height)
    elif format_ == "mono1":
        return decode_mono1(data, width, height)
    else:
        raise ValueError(f"Unsupported format: {format_}")


def image_to_png_bytes(image: Image.Image) -> bytes:
    """Encode PIL image to PNG bytes."""
    output = BytesIO()
    image.save(output, format="PNG")
    return output.getvalue()


def image_to_jpeg_bytes(image: Image.Image, quality: int = 85) -> bytes:
    """Encode PIL image to JPEG bytes.

    Converts grayscale to RGB for consistent JPEG output.
    """
    output = BytesIO()
    if image.mode == "L":
        image = image.convert("RGB")
    image.save(output, format="JPEG", quality=quality)
    return output.getvalue()
