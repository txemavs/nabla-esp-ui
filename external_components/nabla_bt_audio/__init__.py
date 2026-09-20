"""Experimental original-ESP32 HFP Audio Gateway service bring-up."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import esp32
from esphome.const import CONF_ID
DEPENDENCIES = ["esp32"]
CONFLICTS_WITH = ["nabla_bt_keyboard", "esp32_ble", "esp32_ble_tracker", "bluetooth_proxy"]
ns = cg.esphome_ns.namespace("nabla_bt_audio")
Gateway = ns.class_("Gateway", cg.Component)
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Gateway),
    cv.Optional("peer"): cv.mac_address,
}).extend(cv.COMPONENT_SCHEMA)
def validate_final(config):
    from esphome import final_validate as fv
    target = fv.full_config.get()["esp32"]
    if target.get("variant", "ESP32") != "ESP32" or target["framework"]["type"] != "esp-idf":
        raise cv.Invalid("HFP requires original ESP32 with ESP-IDF")
    return config
FINAL_VALIDATE_SCHEMA = validate_final
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if "peer" in config:
        cg.add(var.set_peer(config["peer"].as_hex))
    for name, value in {
        "CONFIG_BT_ENABLED": True, "CONFIG_BT_BLUEDROID_ENABLED": True,
        "CONFIG_BT_CLASSIC_ENABLED": True, "CONFIG_BT_BLE_ENABLED": False,
        "CONFIG_BT_HFP_ENABLE": True, "CONFIG_BT_HFP_AG_ENABLE": True,
        # Keep IDF service-record capacity used by the validated AG connection.
        "CONFIG_BT_HFP_CLIENT_ENABLE": True,
        "CONFIG_BTDM_CTRL_BR_EDR_SCO_DATA_PATH_HCI": True,
        "CONFIG_BTDM_CTRL_BR_EDR_SCO_DATA_PATH_PCM": False,
        "CONFIG_BT_HFP_AUDIO_DATA_PATH_HCI": True,
        "CONFIG_BT_HFP_WBS_ENABLE": False,
        "CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY": True,
    }.items():
        esp32.add_idf_sdkconfig_option(name, value)
