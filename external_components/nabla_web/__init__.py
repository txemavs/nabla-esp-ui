# Headless HTML renderer with shared navigation and transactional Wi-Fi.
from esphome.core import CORE
from esphome import final_validate as fv
from esphome.components.nabla_navigation.catalog import flatten
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
DEPENDENCIES = ["esp32", "wifi", "nabla_navigation", "nabla_wifi_compact"]
AUTO_LOAD = ["web_server_base", "json"]
Web = cg.esphome_ns.namespace("nabla_web").class_("Web", cg.Component)
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Web),
    cv.Optional("camera_port", default=8080): cv.port,
    cv.Required("camera_node"): cv.string_strict,
}).extend(cv.COMPONENT_SCHEMA)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_serve_root("nabla_web_service" not in CORE.config))
    cg.add(var.set_camera_port(config["camera_port"]))
    nodes=flatten(CORE.config["nabla_navigation"]["tree"])
    if any(n["action"] not in ("open", "wifi") for n in nodes):
        raise cv.Invalid("Web preview supports open, information and Wi-Fi nodes only")
    matches=[i for i,n in enumerate(nodes) if n["key"]==config["camera_node"]]
    if not matches:
        raise cv.Invalid("camera_node must refer to an existing navigation key")
    cg.add(var.set_camera_node(matches[0]))

def validate_final(config):
 full=fv.full_config.get()
 if "nabla_display_mirror" in full and "nabla_web_service" not in full:
  raise cv.Invalid("Combined renderers require nabla_web_service")
 return config
FINAL_VALIDATE_SCHEMA=validate_final
