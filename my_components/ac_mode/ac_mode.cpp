#include "esphome/core/log.h"
#include "ac_mode.h"

namespace esphome
{
    namespace ac_mode
    {
        static const char *TAG = "ac_mode.button";

        void a(bool b)
        {
        }

        void ACButton::setup()
        {
        }
        
        void ACButton::press_action()
        {
            climate_->execute_update(climate_->get_messages()->toggleMode);
        }

        /*
        static int command = 0x3;

        void ACButton::press_action()
        {
            auto sender = IRsend(0);
            // Just testing commands
            climate_->execute_update(sender.encodeNEC(0x20, command));
            ESP_LOGD(TAG, "Ran command %x", command);
            command++;
        }
        */

        void ACButton::dump_config()
        {
            ESP_LOGCONFIG(TAG, "Toggle mode switch dump");
        }

    } // namespace ac_mode
} // namespace esphome