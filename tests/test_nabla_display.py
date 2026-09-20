# Unit tests for Nabla Display Home Assistant integration frame decoding.
# Validates RGB332 and mono1 decode against the contract spec.
# Related: https://github.com/txemavs/nabla-esp-ui/issues/34 Phase 2.

import importlib.util
from io import BytesIO
from pathlib import Path
import sys
import unittest

from PIL import Image

path = Path(__file__).resolve().parents[1] / "homeassistant/custom_components/nabla_display/frame.py"
spec = importlib.util.spec_from_file_location("nabla_display_frame", path)
frame = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = frame
spec.loader.exec_module(frame)


class RGB332DecodeTests(unittest.TestCase):
    """Tests for RGB332 frame decoding."""

    def test_rgb332_to_rgb888_primary_colors(self):
        """Verify RGB332 bit extraction for primary colors."""
        self.assertEqual(frame.rgb332_to_rgb888(0b11100000), (255, 0, 0))
        self.assertEqual(frame.rgb332_to_rgb888(0b00011100), (0, 255, 0))
        self.assertEqual(frame.rgb332_to_rgb888(0b00000011), (0, 0, 255))

    def test_rgb332_to_rgb888_extremes(self):
        """Verify RGB332 extremes (black and white)."""
        self.assertEqual(frame.rgb332_to_rgb888(0b00000000), (0, 0, 0))
        self.assertEqual(frame.rgb332_to_rgb888(0b11111111), (255, 255, 255))

    def test_rgb332_to_rgb888_mid_values(self):
        """Verify mid-range RGB332 values scale correctly."""
        mid = frame.rgb332_to_rgb888(0b01001001)
        self.assertTrue(all(64 <= c <= 128 for c in mid))

    def test_decode_rgb332_kit1_size(self):
        """Kit1 160x128 frame should decode to correct image size."""
        width, height = 160, 128
        data = bytes(width * height)
        image = frame.decode_rgb332(data, width, height)
        self.assertEqual(image.size, (width, height))
        self.assertEqual(image.mode, "RGB")

    def test_decode_rgb332_gradient_corners(self):
        """Verify decoded gradient has expected corner colors."""
        width, height = 160, 128
        data = bytearray(width * height)
        data[0] = 0b11100000
        data[width - 1] = 0b00011100
        data[(height - 1) * width] = 0b00000011
        data[-1] = 0b11111111

        image = frame.decode_rgb332(bytes(data), width, height)
        self.assertEqual(image.getpixel((0, 0)), (255, 0, 0))
        self.assertEqual(image.getpixel((width - 1, 0)), (0, 255, 0))
        self.assertEqual(image.getpixel((0, height - 1)), (0, 0, 255))
        self.assertEqual(image.getpixel((width - 1, height - 1)), (255, 255, 255))

    def test_decode_rgb332_size_mismatch(self):
        """Decode should reject incorrect data size."""
        with self.assertRaises(ValueError):
            frame.decode_rgb332(bytes(100), 160, 128)


class Mono1DecodeTests(unittest.TestCase):
    """Tests for mono1 frame decoding."""

    def test_decode_mono1_tcall_size(self):
        """T-Call 128x64 frame should decode to correct image size."""
        width, height = 128, 64
        data = bytes((width * height + 7) // 8)
        image = frame.decode_mono1(data, width, height)
        self.assertEqual(image.size, (width, height))
        self.assertEqual(image.mode, "L")

    def test_decode_mono1_msb_first(self):
        """Verify MSB-first bit order per contract."""
        data = bytes([0b10000001])
        image = frame.decode_mono1(data, 8, 1)
        self.assertEqual(image.getpixel((0, 0)), 255)
        self.assertEqual(image.getpixel((1, 0)), 0)
        self.assertEqual(image.getpixel((6, 0)), 0)
        self.assertEqual(image.getpixel((7, 0)), 255)

    def test_decode_mono1_checkerboard(self):
        """Verify checkerboard pattern decodes correctly."""
        width, height = 8, 2
        data = bytes([0b10101010, 0b01010101])
        image = frame.decode_mono1(data, width, height)

        self.assertEqual(image.getpixel((0, 0)), 255)
        self.assertEqual(image.getpixel((1, 0)), 0)
        self.assertEqual(image.getpixel((0, 1)), 0)
        self.assertEqual(image.getpixel((1, 1)), 255)

    def test_decode_mono1_size_mismatch(self):
        """Decode should reject incorrect data size."""
        with self.assertRaises(ValueError):
            frame.decode_mono1(bytes(100), 128, 64)


class FrameDecodeTests(unittest.TestCase):
    """Tests for the format-dispatching decode_frame function."""

    def test_decode_frame_rgb332(self):
        """decode_frame with rgb332 format."""
        data = bytes(160 * 128)
        image = frame.decode_frame(data, 160, 128, "rgb332")
        self.assertEqual(image.mode, "RGB")

    def test_decode_frame_mono1(self):
        """decode_frame with mono1 format."""
        data = bytes((128 * 64 + 7) // 8)
        image = frame.decode_frame(data, 128, 64, "mono1")
        self.assertEqual(image.mode, "L")

    def test_decode_frame_unknown_format(self):
        """decode_frame should reject unknown formats."""
        with self.assertRaises(ValueError):
            frame.decode_frame(bytes(100), 10, 10, "rgb565")


class ImageEncodingTests(unittest.TestCase):
    """Tests for PNG/JPEG encoding."""

    def test_image_to_png_roundtrip(self):
        """PNG encode/decode preserves image data."""
        original = Image.new("RGB", (100, 100), "red")
        png_bytes = frame.image_to_png_bytes(original)
        decoded = Image.open(BytesIO(png_bytes))
        self.assertEqual(decoded.size, (100, 100))
        self.assertEqual(decoded.getpixel((50, 50)), (255, 0, 0))

    def test_image_to_jpeg_from_grayscale(self):
        """JPEG encoding converts grayscale to RGB."""
        original = Image.new("L", (100, 100), 128)
        jpeg_bytes = frame.image_to_jpeg_bytes(original)
        decoded = Image.open(BytesIO(jpeg_bytes))
        self.assertEqual(decoded.mode, "RGB")

    def test_image_to_png_mono1_frame(self):
        """Full pipeline: mono1 bytes → PIL → PNG."""
        width, height = 128, 64
        data = bytes([0xAA] * ((width * height + 7) // 8))
        image = frame.decode_mono1(data, width, height)
        png_bytes = frame.image_to_png_bytes(image)

        self.assertGreater(len(png_bytes), 0)
        decoded = Image.open(BytesIO(png_bytes))
        self.assertEqual(decoded.size, (width, height))

    def test_image_to_png_rgb332_frame(self):
        """Full pipeline: rgb332 bytes → PIL → PNG."""
        width, height = 160, 128
        data = bytes(range(256)) * (width * height // 256)
        image = frame.decode_rgb332(data, width, height)
        png_bytes = frame.image_to_png_bytes(image)

        self.assertGreater(len(png_bytes), 0)
        decoded = Image.open(BytesIO(png_bytes))
        self.assertEqual(decoded.size, (width, height))


class FrameSizeTests(unittest.TestCase):
    """Tests for frame size calculations matching contract."""

    def test_kit1_frame_size(self):
        """Kit1 160x128 rgb332 = 20480 bytes."""
        self.assertEqual(160 * 128, 20480)

    def test_tcall_frame_size(self):
        """T-Call 128x64 mono1 = 1024 bytes."""
        self.assertEqual((128 * 64 + 7) // 8, 1024)


class ProfileInferenceTests(unittest.TestCase):
    """Tests for fallback profile inference from frame size."""

    def test_infer_tcall_from_1024_bytes(self):
        """1024 bytes should infer T-Call 128x64 mono1."""
        profile = frame.infer_profile_from_size(1024)
        self.assertIsNotNone(profile)
        self.assertEqual(profile["width"], 128)
        self.assertEqual(profile["height"], 64)
        self.assertEqual(profile["format"], "mono1")
        self.assertEqual(profile["name"], "T-Call")

    def test_infer_kit1_from_20480_bytes(self):
        """20480 bytes should infer Kit1 160x128 rgb332."""
        profile = frame.infer_profile_from_size(20480)
        self.assertIsNotNone(profile)
        self.assertEqual(profile["width"], 160)
        self.assertEqual(profile["height"], 128)
        self.assertEqual(profile["format"], "rgb332")
        self.assertEqual(profile["name"], "Kit1")

    def test_infer_twatch_from_57600_bytes(self):
        """57600 bytes should infer T-Watch 240x240 rgb332."""
        profile = frame.infer_profile_from_size(57600)
        self.assertIsNotNone(profile)
        self.assertEqual(profile["width"], 240)
        self.assertEqual(profile["height"], 240)
        self.assertEqual(profile["format"], "rgb332")

    def test_infer_large_from_153600_bytes(self):
        """153600 bytes should infer Large 480x320 rgb332."""
        profile = frame.infer_profile_from_size(153600)
        self.assertIsNotNone(profile)
        self.assertEqual(profile["width"], 480)
        self.assertEqual(profile["height"], 320)
        self.assertEqual(profile["format"], "rgb332")

    def test_infer_unknown_size_returns_none(self):
        """Unknown frame size should return None."""
        self.assertIsNone(frame.infer_profile_from_size(12345))
        self.assertIsNone(frame.infer_profile_from_size(0))
        self.assertIsNone(frame.infer_profile_from_size(999))

    def test_inferred_profile_decodes_correctly(self):
        """Inferred profile should decode matching frame data."""
        profile = frame.infer_profile_from_size(1024)
        data = bytes([0xAA] * 1024)
        image = frame.decode_frame(
            data, profile["width"], profile["height"], profile["format"]
        )
        self.assertEqual(image.size, (128, 64))
        self.assertEqual(image.mode, "L")

    def test_inferred_kit1_decodes_correctly(self):
        """Inferred Kit1 profile should decode matching frame data."""
        profile = frame.infer_profile_from_size(20480)
        data = bytes(20480)
        image = frame.decode_frame(
            data, profile["width"], profile["height"], profile["format"]
        )
        self.assertEqual(image.size, (160, 128))
        self.assertEqual(image.mode, "RGB")


if __name__ == "__main__":
    unittest.main()
