"""Export shared helpers from the component checkout, including remote Git sources."""
from pathlib import Path

import esphome.config_validation as cv
from esphome.core import CORE, CoroPriority, coroutine_with_priority
from esphome.core.config import include_file

MULTI_CONF = True
HEADERS = ['adapters/mqtt-lights/model.h', 'components/light-card/light_card.h', 'components/appearance/fonts.h', 'components/compact-shell/model.h', 'components/compact-shell/render.h', 'components/device-info/runtime.h', 'components/forms/compact_wifi.h', 'components/forms/latin_keyboard.h', 'components/forms/regular.h', 'components/forms/wifi_flow.h', 'components/logo/motion.h', 'components/wifi-form/form.h', 'navigation/focus.h', 'navigation/orientation.h', 'navigation/tree.h', 'profiles/geometry.h']

CONFIG_SCHEMA = cv.Schema({
    cv.Required("headers"): cv.All(cv.ensure_list(cv.one_of(*HEADERS)), cv.Length(min=1)),
})

@coroutine_with_priority(CoroPriority.FINAL)
async def export_headers(headers):
    root = Path(__file__).resolve().parents[2]
    for relative in headers:
        include_file(root / relative, Path(Path(relative).name))

async def to_code(config):
    if "components/light-card/light_card.h" in config["headers"]:
        from esphome.components.lvgl import defines as df
        df.add_define("LV_USE_SLIDER")
        df.add_define("LV_USE_BAR")
    CORE.add_job(export_headers, config["headers"])
