"""Bounded background HTTPS JPEG fetcher for ESP32 regular displays."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import esp32, runtime_image
from esphome.const import CONF_ID, CONF_TRIGGER_ID

DEPENDENCIES = ["esp32", "wifi"]
AUTO_LOAD = ["runtime_image"]
ns = cg.esphome_ns.namespace("nabla_camera_fetch")
Fetcher = ns.class_("Fetcher", cg.Component)
ResultTrigger = ns.class_("ResultTrigger", automation.Trigger.template(cg.int_, cg.bool_))
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Fetcher),
    cv.Required("images"): cv.All(
        cv.ensure_list(cv.use_id(runtime_image.RuntimeImage)), cv.Length(min=1, max=2)),
    cv.Optional("on_result"): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ResultTrigger),
    }),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    for image_id in config["images"]:
        image = await cg.get_variable(image_id)
        cg.add(var.add_image(image))
    for conf in config.get("on_result", []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.int_, "slot"), (cg.bool_, "success")], conf)
    esp32.add_idf_sdkconfig_option("CONFIG_MBEDTLS_CERTIFICATE_BUNDLE", True)
    esp32.add_idf_sdkconfig_option("CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_FULL", True)
