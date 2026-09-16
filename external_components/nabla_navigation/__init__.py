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

def validate_composition(config):
    if any(n["action"] == "forms_demo" for n in flatten(config["tree"])) and "forms" not in config:
        raise cv.Invalid("forms_demo requires forms metadata")
    return config

CONFIG_SCHEMA = cv.All(cv.Schema({
    cv.Required("tree"): validate_tree,
    cv.Optional("forms"): validate_forms,
}), validate_composition)

async def to_code(config):
    cg.add_global(cg.RawStatement(emit(config["tree"])))
    cg.add_global(cg.RawStatement(form_schema.emit(config.get("forms", []))))
