import re
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import esp32
from esphome.const import CONF_ID
DEPENDENCIES = ["esp32", "wifi"]
AUTO_LOAD = ["json"]
MULTI_CONF = True
ns = cg.esphome_ns.namespace("nabla_ha")
Client = ns.class_("Client", cg.Component)
def url(value):
    value = cv.string_strict(value).rstrip("/")
    if not re.fullmatch(r"https://[A-Za-z0-9.-]+(?::[0-9]+)?", value):
        raise cv.Invalid("Use an HTTPS origin without credentials, path or query")
    return value
def entity(value):
    value = cv.string_strict(value)
    if not re.fullmatch(r"(light|switch)\.[a-z0-9_]+", value):
        raise cv.Invalid("Only explicitly configured light/switch entity IDs are accepted")
    return value
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Client),
    cv.Required("url"): url,
    cv.Required("token"): cv.string_strict,
    cv.Required("entities"): cv.All(cv.ensure_list(entity), cv.Length(min=1,max=8)),
}).extend(cv.COMPONENT_SCHEMA)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_url(config["url"]))
    cg.add(var.set_token(config["token"]))
    for entity_id in config["entities"]:
        cg.add(var.add_entity(entity_id))
    esp32.add_idf_sdkconfig_option("CONFIG_MBEDTLS_CERTIFICATE_BUNDLE", True)
    esp32.add_idf_sdkconfig_option("CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_FULL", True)
