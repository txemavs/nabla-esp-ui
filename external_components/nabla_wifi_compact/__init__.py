import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import final_validate as fv
from esphome.core import CORE, ID
from esphome.components import wifi
from esphome.const import CONF_ID, CONF_NETWORKS, CONF_MANUAL_IP, CONF_PRIORITY

# Compact variant: no LVGL dependency, works with compact-shell renderer.
DEPENDENCIES = ["wifi", "nabla_navigation", "esp32"]

ns = cg.esphome_ns.namespace("nabla_wifi")
Adapter = ns.class_("Adapter", cg.Component)
CONFIG_SCHEMA = cv.Schema({cv.GenerateID(): cv.declare_id(Adapter)}).extend(cv.COMPONENT_SCHEMA)

def validate_final(config):
    networks=fv.full_config.get()["wifi"].get(CONF_NETWORKS, [])
    if not 1 <= len(networks) <= 8:
        raise cv.Invalid("nabla_wifi_compact requires 1..8 configured fallback Wi-Fi networks")
    for network in networks:
        if network.get(CONF_PRIORITY, 0)>110:
            raise cv.Invalid("Fallback priorities must be <=110 to reserve primary priority")
    return config
FINAL_VALIDATE_SCHEMA = validate_final

async def to_code(config):
    var=cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var,config)
    cg.add_define("NABLA_WIFI_COMPACT")
    wifi.request_wifi_scan_results()
    wifi.request_wifi_scan_results_listener()
    wifi_config=CORE.config["wifi"]
    for index, network in enumerate(wifi_config[CONF_NETWORKS]):
        def add(ap, entry):
            cg.add(var.add_fallback(wifi.wifi_network(entry, ap, entry.get(CONF_MANUAL_IP, wifi_config.get(CONF_MANUAL_IP)))))
        cg.with_local_variable(ID(f"nabla_fallback_{index}", type=wifi.WiFiAP),wifi.WiFiAP(),add,network)
