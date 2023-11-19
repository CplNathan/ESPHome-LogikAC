#include "esphome/core/log.h"
#include "ac_switch.h"

namespace esphome
{
    namespace ac_switch
    {
        static IRsend *irsend;
        static IRMessages *irmessages;
        static const char *TAG = "ac_switch.switch";

        void a(bool b)
        {
        }

        void ACSwitch::setup()
        {
            this->set_restore_mode(esphome::switch_::SwitchRestoreMode::SWITCH_RESTORE_DISABLED);
            this->add_on_state_callback(a);

            irsend = new IRsend(((InternalGPIOPin *)this->pin_)->get_pin());
            irsend->begin();

            irmessages = new IRMessages(irsend);
        }

        void ACSwitch::write_state(bool state)
        {
            irsend->sendNEC(irmessages->togglePower);

            delay(10);

            this->flush();
            while (!this->available())
            {
            }

            int i = 0;
            while (this->available() && i < 128)
            {
                uint8_t buff = this->read();

                ESP_LOGD(TAG, "%x", buff);
                i++;
            }

            irsend->sendNEC(irmessages->togglePower);

            i = 0;
            while (this->available() && i < 128)
            {
                uint8_t buff = this->read();

                ESP_LOGD(TAG, "%x", buff);
                i++;
            }
            // publish_state()
            // assumed_state()
        }

        void ACSwitch::dump_config()
        {
            ESP_LOGCONFIG(TAG, "Empty custom switch");
        }

    } // namespace empty_switch
} // namespace esphome