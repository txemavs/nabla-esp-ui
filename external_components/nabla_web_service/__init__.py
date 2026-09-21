# One root route for optional responsive and physical-mirror renderers.
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import final_validate as fv
from esphome.const import CONF_ID
DEPENDENCIES=["esp32","wifi"]
CONFLICTS_WITH=["web_server"]
AUTO_LOAD=["web_server_base"]
Service=cg.esphome_ns.namespace("nabla_web_service").class_("Service",cg.Component)
CONFIG_SCHEMA=cv.Schema({
 cv.GenerateID():cv.declare_id(Service),
 cv.Required("mode"):cv.one_of("responsive","mirror","both"),
}).extend(cv.COMPONENT_SCHEMA)
def validate_final(config):
 full=fv.full_config.get()
 for component,needed in [("nabla_web",config["mode"]!="mirror"),("nabla_display_mirror",config["mode"]!="responsive")]:
  if (component in full)!=needed:
   raise cv.Invalid("mode must match the explicitly included web renderer components: "+component)
 return config
FINAL_VALIDATE_SCHEMA=validate_final
async def to_code(config):
 var=cg.new_Pvariable(config[CONF_ID])
 await cg.register_component(var,config)
 cg.add(var.set_mode({"responsive":0,"mirror":1,"both":2}[config["mode"]]))
