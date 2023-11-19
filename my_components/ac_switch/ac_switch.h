#pragma once

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/switch/switch.h"
#include "IRremoteESP8266.h"
#include "IRsend.h"

namespace esphome
{
  namespace ac_switch
  {
    class ACSwitch : public switch_::Switch, public uart::UARTDevice, public Component
    {
    public:
      void setup() override;
      void write_state(bool state) override;
      void set_pin(GPIOPin *pin) { pin_ = pin; }
      void dump_config() override;

    protected:
      GPIOPin *pin_;
    };

    struct IRMessages
    {
    public:
      uint32_t togglePower;

      IRMessages(IRsend *ir)
      {
        togglePower = ir->encodeNEC(0x20, 0x2);
      }
    };
  } // namespace empty_switch
} // namespace esphome