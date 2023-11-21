#pragma once

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/button/button.h"
#include "esphome/components/ac_climate/ac_climate.h"

namespace esphome
{
  namespace ac_mode
  {
    class ACButton : public button::Button, public Component
    {
    public:
      void setup() override;
      void press_action() override;
      void set_climate(ac_climate::AC *climate) { climate_ = climate; }
      void dump_config() override;

    protected:
      ac_climate::AC *climate_;
    };
  } // namespace ac_mode
} // namespace esphome