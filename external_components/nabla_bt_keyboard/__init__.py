"""Optional classic Bluetooth HID boot-keyboard host for the original ESP32."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import esp32
from esphome.const import CONF_ID
DEPENDENCIES = ["esp32"]
CONFLICTS_WITH = ["esp32_ble", "esp32_ble_tracker", "bluetooth_proxy"]
ns = cg.esphome_ns.namespace("nabla_bt_keyboard")
Keyboard = ns.class_("Keyboard", cg.Component)
KeyTrigger = ns.class_("KeyTrigger", automation.Trigger.template(cg.uint8, cg.uint8))
CONFIG_SCHEMA = cv.All(cv.Schema({
    cv.GenerateID(): cv.declare_id(Keyboard),
    cv.Optional("on_key"): automation.validate_automation({
        cv.GenerateID("trigger_id"): cv.declare_id(KeyTrigger),
    }),
}).extend(cv.COMPONENT_SCHEMA))
def validate_final(config):
    from esphome import final_validate as fv
    target = fv.full_config.get()["esp32"]
    if target.get("variant", "ESP32") != "ESP32" or target["framework"]["type"] != "esp-idf":
        raise cv.Invalid("Classic Bluetooth requires the original ESP32; S3/C3 are not supported")
    return config
FINAL_VALIDATE_SCHEMA = validate_final
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    for option, value in {
        "CONFIG_BT_ENABLED": True, "CONFIG_BT_BLUEDROID_ENABLED": True,
        "CONFIG_BT_CLASSIC_ENABLED": True, "CONFIG_BT_BLE_ENABLED": False,
        "CONFIG_BT_HID_ENABLED": True, "CONFIG_BT_HID_HOST_ENABLED": True, "CONFIG_BT_SSP_ENABLED": True,
        "CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY": True,
    }.items():
        esp32.add_idf_sdkconfig_option(option, value)
    for conf in config.get("on_key", []):
        trigger = cg.new_Pvariable(conf["trigger_id"], var)
        await automation.build_automation(trigger, [(cg.uint8, "key"), (cg.uint8, "modifiers")], conf)
