import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import button
from esphome.const import CONF_ID, CONF_ENTITY_ID

ac_mode_ns = cg.esphome_ns.namespace('ac_mode')
ACButton = ac_mode_ns.class_('ACButton', button.Button, cg.Component)

CONFIG_SCHEMA = (button.BUTTON_SCHEMA
.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(ACButton),
        cv.Required(CONF_ENTITY_ID): cv.use_id(CONF_ENTITY_ID)
    }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

def to_code(config):    
    var = cg.new_Pvariable(config[CONF_ID])

    climate = yield cg.get_variable(config[CONF_ENTITY_ID])
    cg.add(var.set_climate(climate))

    yield cg.register_component(var, config)
    yield button.register_button(var, config)