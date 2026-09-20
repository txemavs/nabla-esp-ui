# Home Assistant custom component for Nabla Display mirror.
# Polls ESP devices for display frames and exposes encoder actions.
# See docs/platform/DISPLAY-MIRROR-CONTRACT.md for the HTTP contract.

import asyncio
import logging
import time

import aiohttp
from aiohttp import web
import voluptuous as vol

from homeassistant.const import EVENT_HOMEASSISTANT_STOP, Platform
from homeassistant.core import HomeAssistant, ServiceCall
from homeassistant.helpers import config_validation as cv, discovery
from homeassistant.helpers.aiohttp_client import async_get_clientsession

from .frame import decode_frame, image_to_png_bytes

_LOGGER = logging.getLogger(__name__)

DOMAIN = "nabla_display"

CONF_DEVICES = "devices"
CONF_HOST = "host"
CONF_NAME = "name"
CONF_POLL_INTERVAL = "poll_interval"

DEFAULT_POLL_INTERVAL = 1.0

DEVICE_SCHEMA = vol.Schema({
    vol.Required(CONF_HOST): cv.string,
    vol.Optional(CONF_NAME): cv.string,
    vol.Optional(CONF_POLL_INTERVAL, default=DEFAULT_POLL_INTERVAL):
        vol.All(vol.Coerce(float), vol.Range(min=0.2, max=30)),
})

CONFIG_SCHEMA = vol.Schema({
    DOMAIN: vol.Schema({
        vol.Required(CONF_DEVICES): vol.All(cv.ensure_list, [DEVICE_SCHEMA]),
    })
}, extra=vol.ALLOW_EXTRA)


class DeviceState:
    """Tracks state for one Nabla Display device."""

    def __init__(self, host: str, name: str, poll_interval: float, session: aiohttp.ClientSession):
        self.host = host
        self.name = name
        self.poll_interval = poll_interval
        self.session = session

        self.capabilities: dict | None = None
        self.token: str | None = None
        self.last_frame: bytes | None = None
        self.last_png: bytes | None = None
        self.last_update: float = 0
        self.available = False
        self.poll_task: asyncio.Task | None = None

    @property
    def device_id(self) -> str:
        """Stable ID derived from host."""
        return self.host.replace(".", "_").replace(":", "_")

    @property
    def width(self) -> int:
        return self.capabilities.get("width", 0) if self.capabilities else 0

    @property
    def height(self) -> int:
        return self.capabilities.get("height", 0) if self.capabilities else 0

    @property
    def format(self) -> str:
        return self.capabilities.get("format", "unknown") if self.capabilities else "unknown"

    @property
    def has_input(self) -> bool:
        return self.capabilities.get("input", False) if self.capabilities else False

    async def fetch_capabilities(self) -> bool:
        """Fetch device capabilities. Returns True on success."""
        try:
            url = f"http://{self.host}/mirror/capabilities"
            async with self.session.get(url, timeout=aiohttp.ClientTimeout(total=5)) as resp:
                if resp.status == 200:
                    self.capabilities = await resp.json()
                    _LOGGER.debug("Capabilities for %s: %s", self.name, self.capabilities)
                    return True
                _LOGGER.warning("Capabilities fetch failed for %s: HTTP %d", self.name, resp.status)
        except Exception as e:
            _LOGGER.warning("Capabilities fetch failed for %s: %s", self.name, e)
        return False

    async def fetch_token(self) -> bool:
        """Fetch CSRF token for input. Returns True on success."""
        if not self.has_input:
            return True
        try:
            url = f"http://{self.host}/mirror/token"
            async with self.session.get(url, timeout=aiohttp.ClientTimeout(total=5)) as resp:
                if resp.status == 200:
                    self.token = await resp.text()
                    _LOGGER.debug("Token fetched for %s", self.name)
                    return True
                _LOGGER.warning("Token fetch failed for %s: HTTP %d", self.name, resp.status)
        except Exception as e:
            _LOGGER.warning("Token fetch failed for %s: %s", self.name, e)
        return False

    async def fetch_frame(self) -> bool:
        """Fetch current frame. Returns True on success."""
        if not self.capabilities:
            return False
        try:
            url = f"http://{self.host}/mirror/frame"
            async with self.session.get(url, timeout=aiohttp.ClientTimeout(total=10)) as resp:
                if resp.status == 200:
                    self.last_frame = await resp.read()
                    image = decode_frame(
                        self.last_frame, self.width, self.height, self.format
                    )
                    self.last_png = image_to_png_bytes(image)
                    self.last_update = time.monotonic()
                    self.available = True
                    return True
                elif resp.status == 409:
                    _LOGGER.debug("Frame not ready for %s", self.name)
                else:
                    _LOGGER.warning("Frame fetch failed for %s: HTTP %d", self.name, resp.status)
        except Exception as e:
            _LOGGER.warning("Frame fetch failed for %s: %s", self.name, e)
        self.available = False
        return False

    async def send_action(self, action: str) -> bool:
        """Send encoder action to device. Returns True on success."""
        if not self.has_input or not self.token:
            _LOGGER.warning("Cannot send action to %s: no input or token", self.name)
            return False

        valid_actions = ("up", "down", "enter", "back")
        if action not in valid_actions:
            _LOGGER.warning("Invalid action '%s' for %s", action, self.name)
            return False

        try:
            url = f"http://{self.host}/mirror/action"
            headers = {"X-Nabla-Token": self.token}
            data = {"action": action}
            async with self.session.post(
                url, headers=headers, data=data,
                timeout=aiohttp.ClientTimeout(total=5)
            ) as resp:
                if resp.status == 200:
                    _LOGGER.debug("Action %s sent to %s", action, self.name)
                    return True
                elif resp.status == 401:
                    _LOGGER.warning("Token rejected for %s, refetching", self.name)
                    await self.fetch_token()
                else:
                    _LOGGER.warning("Action failed for %s: HTTP %d", self.name, resp.status)
        except Exception as e:
            _LOGGER.warning("Action failed for %s: %s", self.name, e)
        return False

    async def poll_loop(self):
        """Background polling loop for frame updates."""
        while True:
            if not self.capabilities:
                if not await self.fetch_capabilities():
                    await asyncio.sleep(5)
                    continue
                await self.fetch_token()

            await self.fetch_frame()
            await asyncio.sleep(self.poll_interval)

    def start_polling(self):
        """Start the background polling task."""
        if self.poll_task is None:
            self.poll_task = asyncio.create_task(self.poll_loop())

    async def stop_polling(self):
        """Stop the background polling task."""
        if self.poll_task:
            self.poll_task.cancel()
            try:
                await self.poll_task
            except asyncio.CancelledError:
                pass
            self.poll_task = None


async def async_setup(hass: HomeAssistant, config: dict) -> bool:
    """Set up the Nabla Display integration."""
    conf = config.get(DOMAIN)
    if not conf:
        return True

    session = async_get_clientsession(hass)
    devices: dict[str, DeviceState] = {}

    for device_conf in conf[CONF_DEVICES]:
        host = device_conf[CONF_HOST]
        name = device_conf.get(CONF_NAME, f"Nabla Display {host}")
        poll_interval = device_conf.get(CONF_POLL_INTERVAL, DEFAULT_POLL_INTERVAL)

        device = DeviceState(host, name, poll_interval, session)
        devices[device.device_id] = device
        device.start_polling()

    hass.data[DOMAIN] = {"devices": devices}

    hass.http.register_view(FrameImageView(devices))

    async def handle_send_action(call: ServiceCall):
        """Handle nabla_display.send_action service call."""
        device_id = call.data.get("device_id")
        action = call.data.get("action")

        if device_id not in devices:
            _LOGGER.error("Unknown device: %s", device_id)
            return

        await devices[device_id].send_action(action)

    hass.services.async_register(
        DOMAIN, "send_action", handle_send_action,
        schema=vol.Schema({
            vol.Required("device_id"): cv.string,
            vol.Required("action"): vol.In(["up", "down", "enter", "back"]),
        })
    )

    await discovery.async_load_platform(hass, Platform.BUTTON, DOMAIN, {}, config)

    async def stop(_event):
        for device in devices.values():
            await device.stop_polling()

    hass.bus.async_listen_once(EVENT_HOMEASSISTANT_STOP, stop)

    return True


class FrameImageView(web.View):
    """HTTP view serving device frame images."""

    url = "/api/nabla_display/{device_id}/frame"
    name = "api:nabla_display:frame"

    def __init__(self, devices: dict[str, DeviceState]):
        self.devices = devices

    async def get(self, request: web.Request) -> web.Response:
        """Handle GET request for frame image."""
        device_id = request.match_info.get("device_id")

        if device_id not in self.devices:
            return web.Response(status=404, text="Device not found")

        device = self.devices[device_id]

        if not device.available or device.last_png is None:
            return web.Response(status=503, text="Frame not available")

        return web.Response(
            body=device.last_png,
            content_type="image/png",
            headers={
                "Cache-Control": "no-store",
                "X-Nabla-Width": str(device.width),
                "X-Nabla-Height": str(device.height),
                "X-Nabla-Format": device.format,
                "X-Nabla-Input": "1" if device.has_input else "0",
            }
        )
