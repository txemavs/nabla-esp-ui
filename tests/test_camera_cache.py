"""Behavior tests for the optional server-side camera cache."""
import asyncio
import importlib.util
from io import BytesIO
from pathlib import Path
import sys
import unittest

from PIL import Image

path = Path(__file__).resolve().parents[1] / "services/homeassistant/custom_components/nabla_camera_cache/cache.py"
spec = importlib.util.spec_from_file_location("nabla_camera_cache_engine", path)
engine = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = engine
spec.loader.exec_module(engine)


def jpeg(size=(1920, 1080)):
    out = BytesIO()
    Image.new("RGB", size, "red").save(out, "JPEG")
    return out.getvalue()


async def execute(fn, *args):
    return await asyncio.to_thread(fn, *args)


class ImageTests(unittest.TestCase):
    def test_sizes_letterbox_and_baseline(self):
        images = engine.render(jpeg())
        with Image.open(BytesIO(images["view"])) as image:
            self.assertEqual(image.size, (480, 270))
            self.assertFalse(image.info.get("progressive", False))
        with Image.open(BytesIO(images["icon"])) as image:
            self.assertEqual(image.size, (64, 64))
            self.assertLess(max(image.getpixel((32, 0))), 10)
            self.assertGreater(image.getpixel((32, 32))[0], 200)

    def test_portrait_and_no_upscale(self):
        for source, expected in (((1080, 1920), (270, 480)), ((80, 60), (80, 60))):
            with Image.open(BytesIO(engine.render(jpeg(source))["view"])) as image:
                self.assertEqual(image.size, expected)

    def test_bad_and_oversized_sources(self):
        for data in (b"not a jpeg", b"x" * (engine.MAX_SOURCE_BYTES + 1)):
            with self.assertRaises(Exception):
                engine.render(data)


class CacheTests(unittest.IsolatedAsyncioTestCase):
    async def asyncSetUp(self):
        self.now = 100.0
        self.calls = 0
        self.release = asyncio.Event()
        self.release.set()
        self.broken = False

        async def fetch():
            self.calls += 1
            await self.release.wait()
            if self.broken:
                raise RuntimeError("private upstream detail")
            return jpeg()

        self.cache = engine.CameraCache(fetch, execute, asyncio.Semaphore(2),
                                        clock=lambda: self.now)

    async def asyncTearDown(self):
        await self.cache.close()

    async def test_23_readers_share_one_pair_and_idle_does_nothing(self):
        frames = await asyncio.gather(*(self.cache.get() for _ in range(23)))
        self.assertEqual(self.calls, 1)
        self.assertTrue(all(frame is frames[0] for frame in frames))
        self.assertEqual(set(frames[0].images), {"icon", "view"})
        self.now += 10
        await asyncio.sleep(0.02)
        self.assertEqual(self.calls, 1)

    async def test_atomic_warm_refresh(self):
        first = await self.cache.get()
        self.now += 2
        self.release.clear()
        frames = await asyncio.gather(*(self.cache.get() for _ in range(23)))
        await asyncio.sleep(0)
        self.assertEqual(self.calls, 2)
        self.assertTrue(all(frame is first for frame in frames))
        task = self.cache.task
        self.release.set()
        await task
        self.assertEqual((await self.cache.get()).generation, 2)

    async def test_error_backoff_and_stale_limit(self):
        await self.cache.get()
        self.now += 9
        self.broken = True
        for _ in range(23):
            with self.assertRaises(engine.Unavailable):
                await self.cache.get()
        self.assertEqual(self.calls, 2)
        self.now += 3
        self.broken = False
        self.assertEqual((await self.cache.get()).generation, 2)

    async def test_cancelled_reader_does_not_cancel_shared_work(self):
        self.release.clear()
        reader = asyncio.create_task(self.cache.get())
        await asyncio.sleep(0)
        reader.cancel()
        with self.assertRaises(asyncio.CancelledError):
            await reader
        other = asyncio.create_task(self.cache.get())
        self.release.set()
        self.assertEqual((await other).generation, 1)
        self.assertEqual(self.calls, 1)

    async def test_etag_stable_for_identical_frames(self):
        first = await self.cache.get()
        self.now += 2
        await self.cache.get()
        await self.cache.task
        self.assertEqual(first.etags, self.cache.frame.etags)
        self.assertEqual(self.cache.generations, 2)


if __name__ == "__main__":
    unittest.main()
