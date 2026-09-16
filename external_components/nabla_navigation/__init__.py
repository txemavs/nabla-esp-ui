import esphome.codegen as cg
import esphome.config_validation as cv
from .catalog import flatten, emit

def validate_tree(value):
    try:
        flatten(value)
    except ValueError as err:
        raise cv.Invalid(str(err)) from err
    return value

CONFIG_SCHEMA = cv.Schema({cv.Required("tree"): validate_tree})

async def to_code(config):
    cg.add_global(cg.RawStatement(emit(config["tree"])))
