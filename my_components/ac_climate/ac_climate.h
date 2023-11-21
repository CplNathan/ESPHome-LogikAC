#pragma once

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/uart/uart_debugger.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/climate/climate_traits.h"
#include "irmessages.h"
#include "IRremoteESP8266.h"

namespace esphome
{
  namespace ac_climate
  {
    class AC : public climate::Climate, public uart::UARTDevice, public PollingComponent
    {
    public:
      void setup() override;
      void update() override;
      void set_pin(GPIOPin *ir_pin) { ir_pin_ = ir_pin; }
      void dump_config() override;
      IRMessages* get_messages();

      climate::ClimateTraits traits() override;
      void control(const climate::ClimateCall &call) override;
      void execute_update(uint32_t command);

    protected:
      GPIOPin *ir_pin_;
      std::vector<uint8_t> read_state();
      void update_state();
      void execute_updates(const std::vector<uint32_t>& commands);
    };
  } // namespace empty_switch
} // namespace esphome