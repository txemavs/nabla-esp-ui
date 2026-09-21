# Optional logical-pixel display mirror with bounded input forwarding.
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID
from esphome.core import CORE
DEPENDENCIES = ["esp32", "wifi", "display"]
CONFLICTS_WITH = ["web_server"]
AUTO_LOAD = ["web_server_base"]
ns=cg.esphome_ns.namespace("nabla_display_mirror")
Mirror=ns.class_("Mirror",cg.Component)
CONFIG_SCHEMA=cv.Schema({
    cv.GenerateID():cv.declare_id(Mirror),
    cv.Optional("width", default=128):cv.int_range(min=1,max=480),
    cv.Optional("height", default=64):cv.int_range(min=1,max=480),
    cv.Optional("color", default=False):cv.boolean,
    cv.Optional("lvgl_id"):cv.use_id(cg.esphome_ns.namespace("lvgl").class_("LvglComponent",cg.Component)),
    cv.Optional("on_action"):automation.validate_automation(single=True),
}).extend(cv.COMPONENT_SCHEMA)
async def to_code(config):
    var=cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var,config)
    cg.add(var.configure(config["width"],config["height"],config["color"]))
    if "lvgl_id" in config:
        if not config["color"]:
            raise cv.Invalid("LVGL mirror requires color: true")
        cg.add_define("NABLA_MIRROR_LVGL")
        cg.add(var.set_lvgl(await cg.get_variable(config["lvgl_id"])))
    cg.add(var.set_serve_root("nabla_web_service" not in CORE.config))
    cg.add(var.set_allow_input("on_action" in config))
    if "on_action" not in config:
        return
    await automation.build_automation(var.get_action_trigger(),[(cg.std_string,"action")],config["on_action"])
