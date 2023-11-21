#pragma once

#include "IRsend.h"

namespace esphome
{
  namespace ac_climate
  {
    struct IRMessages
    {
    public:
      uint32_t togglePower;
      uint32_t toggleFanSpeed;
      uint32_t toggleMode;
      uint32_t toggleSwing;

      IRMessages(IRsend *ir)
      {
        togglePower = ir->encodeNEC(0x20, 0x2);
        toggleFanSpeed = ir->encodeNEC(0x20, 0x1A);
        toggleMode = ir->encodeNEC(0x20, 0x9);
        toggleSwing = ir->encodeNEC(0x20, 0x1);
      }
    };
  }
}