import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import climate, uart
from esphome.const import CONF_ID, CONF_PIN

DEPENDENCIES = ["uart"]

ac_switch_ns = cg.esphome_ns.namespace('ac_climate')
AC = ac_switch_ns.class_('AC', climate.Climate, uart.UARTDevice, cg.PollingComponent)

CONFIG_SCHEMA = (climate.CLIMATE_SCHEMA
.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(AC),
        cv.Required(CONF_PIN): pins.gpio_output_pin_schema
    }
    )
    .extend(cv.polling_component_schema('10s'))
    .extend(uart.UART_DEVICE_SCHEMA)
)

def to_code(config):
    cg.add_library(
        name="IRRemote",
        repository="https://github.com/crankyoldgit/IRremoteESP8266",
        version="a295f87"
    )
    
    var = cg.new_Pvariable(config[CONF_ID])

    irpin = yield cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(irpin))

    yield cg.register_component(var, config)
    yield climate.register_climate(var, config)
    yield uart.register_uart_device(var, config)