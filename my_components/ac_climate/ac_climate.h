#pragma once

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/uart/uart_debugger.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/climate/climate_traits.h"
#include "IRremoteESP8266.h"
#include "IRsend.h"

namespace esphome
{
  namespace ac_climate
  {
    class AC : public climate::Climate, public uart::UARTDevice, public Component
    {
    public:
      void setup() override;
      void set_pin(GPIOPin *ir_pin) { ir_pin_ = ir_pin; }
      void dump_config() override;

      climate::ClimateTraits traits() override;
      void control(const climate::ClimateCall &call) override;

    protected:
      GPIOPin *ir_pin_;
      std::vector<uint8_t> read_state();
      void update_state();
    };

    struct IRMessages
    {
    public:
      uint32_t togglePower;
      uint32_t toggleFanSpeed;

      IRMessages(IRsend *ir)
      {
        togglePower = ir->encodeNEC(0x20, 0x2);
        toggleFanSpeed = ir->encodeNEC(0x20, 0x1A);
      }
    };
  } // namespace empty_switch
} // namespace esphome