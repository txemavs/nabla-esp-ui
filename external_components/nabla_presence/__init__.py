# Optional MQTT address announcements with a matching HTTP identity endpoint.
import re
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ['esp32', 'wifi', 'mqtt']
AUTO_LOAD = ['web_server_base', 'json']
CONFLICTS_WITH = ['web_server']
Presence = cg.esphome_ns.namespace('nabla_presence').class_('Presence', cg.PollingComponent)


def topic_prefix(value):
    value = cv.string_strict(value)
    if not re.fullmatch(r'[A-Za-z0-9_-]+(?:/[A-Za-z0-9_-]+)*', value) or len(value) > 128:
        raise cv.Invalid('Use a topic prefix without wildcards or empty segments (max 128 characters)')
    return value


CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Presence),
    cv.Optional('topic_prefix', default='nabla/discovery'): topic_prefix,
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_prefix(config['topic_prefix']))
