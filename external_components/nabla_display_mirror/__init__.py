# Optional monochrome display mirror with bounded input forwarding.
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID
DEPENDENCIES = ["esp32", "wifi", "display"]
CONFLICTS_WITH = ["nabla_web", "web_server"]
AUTO_LOAD = ["web_server_base"]
ns=cg.esphome_ns.namespace("nabla_display_mirror")
Mirror=ns.class_("Mirror",cg.Component)
CONFIG_SCHEMA=cv.Schema({
    cv.GenerateID():cv.declare_id(Mirror),
    cv.Required("on_action"):automation.validate_automation(single=True),
}).extend(cv.COMPONENT_SCHEMA)
async def to_code(config):
    var=cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var,config)
    await automation.build_automation(var.get_action_trigger(),[(cg.std_string,"action")],config["on_action"])
