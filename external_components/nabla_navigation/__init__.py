from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from .catalog import flatten, emit
from . import form_schema

def validate_tree(value):
    try:
        flatten(value)
    except ValueError as err:
        raise cv.Invalid(str(err)) from err
    return value


def validate_forms(value):
    try:
        return form_schema.validate(value)
    except ValueError as err:
        raise cv.Invalid(str(err)) from err

CommandTrigger = cg.esphome_ns.namespace("nabla_navigation").class_("CommandTrigger", automation.Trigger.template())
COMMAND_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CommandTrigger),
    cv.Required("node"): cv.string_strict,
    cv.Required("available"): cv.returning_lambda,
    cv.Required("state"): cv.returning_lambda,
    cv.Required("on_press"): automation.validate_automation(single=True),
})

def validate_composition(config):
    nodes = flatten(config["tree"])
    required = {n["key"] for n in nodes if n["action"] == "command"}
    registered = [c["node"] for c in config.get("commands", [])]
    if len(set(registered)) != len(registered) or set(registered) != required:
        raise cv.Invalid("Every command leaf needs exactly one matching commands entry")

    if any(n["action"] == "forms_demo" for n in flatten(config["tree"])) and "forms" not in config:
        raise cv.Invalid("forms_demo requires forms metadata")
    return config

CONFIG_SCHEMA = cv.All(cv.Schema({
    cv.Required("tree"): validate_tree,
    cv.Optional("forms"): validate_forms,
    cv.Optional("commands"): cv.ensure_list(COMMAND_SCHEMA),
}), validate_composition)

async def to_code(config):
    cg.add_global(cg.RawStatement(emit(config["tree"])))
    cg.add_global(cg.RawStatement(form_schema.emit(config.get("forms", []))))

    nodes = flatten(config["tree"])
    for command in config.get("commands", []):
        index = next(i for i,n in enumerate(nodes) if n["key"] == command["node"])
        var = cg.new_Pvariable(command["id"], index)
        available = await cg.process_lambda(command["available"], [], return_type=cg.bool_)
        state = await cg.process_lambda(command["state"], [], return_type=cg.std_string)
        cg.add(var.set_available(available))
        cg.add(var.set_state(state))
        await automation.build_automation(var, [], command["on_press"])
