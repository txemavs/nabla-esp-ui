# Tests de fixture para el contrato HTTP del mirror de pantalla Nabla.
# Valida decodificación de bytes y esquema de capabilities sin hardware.
# Relacionado: https://github.com/txemavs/nabla-esp-ui/issues/34 Fase 1.

import json
import unittest


def rgb332_to_rgb888(byte: int) -> tuple[int, int, int]:
    """Convierte byte RGB332 a tupla RGB888."""
    r = ((byte >> 5) & 0x07) * 255 // 7
    g = ((byte >> 2) & 0x07) * 255 // 7
    b = (byte & 0x03) * 255 // 3
    return (r, g, b)


def mono1_to_pixels(data: bytes, width: int, height: int) -> list[int]:
    """Convierte buffer mono1 MSB-primero a lista de valores de píxel (0 o 255)."""
    pixels = []
    total = width * height
    for i in range(total):
        byte_idx = i // 8
        bit_idx = 7 - (i % 8)
        pixels.append(255 if (data[byte_idx] >> bit_idx) & 1 else 0)
    return pixels


def generate_rgb332_gradient(width: int, height: int) -> bytes:
    """Genera un patrón de gradiente RGB332 determinista para pruebas."""
    data = bytearray(width * height)
    for y in range(height):
        for x in range(width):
            r = (x * 7 // (width - 1)) if width > 1 else 0
            g = (y * 7 // (height - 1)) if height > 1 else 0
            b = ((x + y) * 3 // (width + height - 2)) if (width + height) > 2 else 0
            data[y * width + x] = (r << 5) | (g << 2) | b
    return bytes(data)


def generate_mono1_checkerboard(width: int, height: int) -> bytes:
    """Genera un patrón de tablero de ajedrez mono1 determinista para pruebas."""
    total_bits = width * height
    total_bytes = (total_bits + 7) // 8
    data = bytearray(total_bytes)
    for i in range(total_bits):
        x = i % width
        y = i // width
        if (x + y) % 2 == 0:
            byte_idx = i // 8
            bit_idx = 7 - (i % 8)
            data[byte_idx] |= (1 << bit_idx)
    return bytes(data)


class DisplayMirrorTests(unittest.TestCase):
    """Tests de decodificación RGB332 y mono1 según DISPLAY-MIRROR-CONTRACT.md."""

    def test_rgb332_byte_layout(self):
        """Verifica extracción de campos de bits RGB332 según spec del contrato."""
        self.assertEqual(rgb332_to_rgb888(0b11100000), (255, 0, 0))
        self.assertEqual(rgb332_to_rgb888(0b00011100), (0, 255, 0))
        self.assertEqual(rgb332_to_rgb888(0b00000011), (0, 0, 255))
        self.assertEqual(rgb332_to_rgb888(0b11111111), (255, 255, 255))
        self.assertEqual(rgb332_to_rgb888(0b00000000), (0, 0, 0))
        mid_gray = rgb332_to_rgb888(0b01001001)
        self.assertTrue(all(64 <= c <= 128 for c in mid_gray))

    def test_rgb332_kit1_frame_size(self):
        """Perfil Kit1: 160x128 = 20480 bytes."""
        width, height = 160, 128
        expected_size = width * height
        self.assertEqual(expected_size, 20480)
        frame = generate_rgb332_gradient(width, height)
        self.assertEqual(len(frame), expected_size)

    def test_rgb332_gradient_decode(self):
        """Decodifica gradiente RGB332 160x128 generado y verifica esquinas."""
        width, height = 160, 128
        frame = generate_rgb332_gradient(width, height)

        tl = rgb332_to_rgb888(frame[0])
        tr = rgb332_to_rgb888(frame[width - 1])
        bl = rgb332_to_rgb888(frame[(height - 1) * width])
        br = rgb332_to_rgb888(frame[-1])

        self.assertEqual(tl[0], 0)
        self.assertEqual(tl[1], 0)
        self.assertEqual(tr[0], 255)
        self.assertEqual(tr[1], 0)
        self.assertEqual(bl[0], 0)
        self.assertEqual(bl[1], 255)
        self.assertEqual(br[0], 255)
        self.assertEqual(br[1], 255)

    def test_mono1_byte_layout(self):
        """Verifica orden de bits mono1 MSB-primero según spec del contrato."""
        data = bytes([0b10000001])
        pixels = mono1_to_pixels(data, 8, 1)
        self.assertEqual(pixels[0], 255)
        self.assertEqual(pixels[7], 255)
        self.assertEqual(pixels[1], 0)
        self.assertEqual(pixels[6], 0)

    def test_mono1_tcall_frame_size(self):
        """Perfil T-Call: 128x64 / 8 = 1024 bytes."""
        width, height = 128, 64
        expected_size = (width * height + 7) // 8
        self.assertEqual(expected_size, 1024)
        frame = generate_mono1_checkerboard(width, height)
        self.assertEqual(len(frame), expected_size)

    def test_mono1_checkerboard_decode(self):
        """Decodifica tablero mono1 128x64 generado y verifica patrón."""
        width, height = 128, 64
        frame = generate_mono1_checkerboard(width, height)
        pixels = mono1_to_pixels(frame, width, height)

        self.assertEqual(len(pixels), width * height)
        self.assertEqual(pixels[0], 255)
        self.assertEqual(pixels[1], 0)
        self.assertEqual(pixels[width], 0)
        self.assertEqual(pixels[width + 1], 255)

    def test_capabilities_schema_rgb332(self):
        """Valida esquema JSON de capabilities clase Kit1."""
        capabilities = {
            "width": 160,
            "height": 128,
            "format": "rgb332",
            "input": True
        }
        self._validate_capabilities(capabilities)
        self.assertEqual(capabilities["format"], "rgb332")
        self.assertEqual(capabilities["width"] * capabilities["height"], 20480)

    def test_capabilities_schema_mono1(self):
        """Valida esquema JSON de capabilities clase T-Call."""
        capabilities = {
            "width": 128,
            "height": 64,
            "format": "mono1",
            "input": True
        }
        self._validate_capabilities(capabilities)
        self.assertEqual(capabilities["format"], "mono1")
        frame_size = (capabilities["width"] * capabilities["height"] + 7) // 8
        self.assertEqual(frame_size, 1024)

    def test_capabilities_schema_readonly(self):
        """Valida capabilities de pantalla solo lectura."""
        capabilities = {
            "width": 240,
            "height": 240,
            "format": "rgb332",
            "input": False
        }
        self._validate_capabilities(capabilities)
        self.assertFalse(capabilities["input"])

    def test_capabilities_json_roundtrip(self):
        """El JSON de capabilities debe sobrevivir serialización."""
        original = {"width": 160, "height": 128, "format": "rgb332", "input": True}
        serialized = json.dumps(original)
        restored = json.loads(serialized)
        self.assertEqual(original, restored)

    def test_frame_size_calculations(self):
        """Verifica fórmulas de tamaño de frame para todos los perfiles documentados."""
        profiles = [
            (128, 64, "mono1", 1024),
            (160, 128, "rgb332", 20480),
            (240, 240, "rgb332", 57600),
            (480, 320, "rgb332", 153600),
        ]
        for width, height, fmt, expected in profiles:
            if fmt == "mono1":
                size = (width * height + 7) // 8
            else:
                size = width * height
            self.assertEqual(size, expected, f"{width}x{height} {fmt}")

    def _validate_capabilities(self, cap: dict):
        """Verifica campos requeridos de capabilities según contrato."""
        self.assertIn("width", cap)
        self.assertIn("height", cap)
        self.assertIn("format", cap)
        self.assertIn("input", cap)
        self.assertIsInstance(cap["width"], int)
        self.assertIsInstance(cap["height"], int)
        self.assertIn(cap["format"], ("mono1", "rgb332"))
        self.assertIsInstance(cap["input"], bool)
        self.assertGreater(cap["width"], 0)
        self.assertGreater(cap["height"], 0)


if __name__ == "__main__":
    unittest.main()
