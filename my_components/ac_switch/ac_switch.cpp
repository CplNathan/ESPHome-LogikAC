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

        std::vector<uint8_t> ACSwitch::read_state()
        {
            int baud = this->parent_->get_baud_rate();
            int databits = this->parent_->get_data_bits();
            int us = 1 / baud * 1000000;

            // Try to clear out the buffer.#
            int available = this->available();
            while (available--)
                this->read();

            std::vector<uint8_t> data = {1, 1, 1, 1, 1, 1, 1, 1};

            while (data[0] != 0x0 && data[1] != 0x12)
            {
                while (!(this->available() > 2))
                {
                }
                data[0] = this->read();
                data[1] = this->peek();
            }

            for (int i = 2; i < 8; i++)
            {
                while (!this->available())
                {
                }
                data[i] = this->read();
            }

            uart::UARTDebug::log_binary(uart::UARTDirection::UART_DIRECTION_RX, data, ',');

            return data;
        }

        void ACSwitch::setup()
        {
            this->set_restore_mode(esphome::switch_::SwitchRestoreMode::SWITCH_RESTORE_DISABLED);
            this->add_on_state_callback(a);

            irsend = new IRsend(((InternalGPIOPin *)this->pin_)->get_pin());
            irsend->begin();

            irmessages = new IRMessages(irsend);

            bool currentlyOn = this->read_state()[3];
            this->publish_state(currentlyOn);
        }

        static int command = 0x3;

        void ACSwitch::write_state(bool state)
        {
            // Just testing commands
            irsend->sendNEC(irsend->encodeNEC(0x20, command));
            ESP_LOGD(TAG, "Ran command %x", command);
            command++;
            this->publish_state(true);
            /*
std::vector<uint8_t> data = read_state();
bool currentlyOn = this->read_state()[3];

if (currentlyOn && state)
{
    this->publish_state(true);
    return;
}

if (!currentlyOn && !state)
{
    this->publish_state(false);
    return;
}

irsend->sendNEC(irmessages->togglePower);

delay(50);

data = read_state();
currentlyOn = this->read_state()[3];

this->publish_state(currentlyOn);

// Todo: Codes for switch mode/sweep/fan speed - try and read state so we can know if its on or off, if we can decode that signal...

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
            delay(10);

            this->flush();
            while (this->available() && i < 128)
            {
                uint8_t buff = this->read();

                ESP_LOGD(TAG, "%x", buff);
                i++;
            }
            */
            // publish_state()
        }

        void ACSwitch::dump_config()
        {
            ESP_LOGCONFIG(TAG, "Empty custom switch");
        }

    } // namespace empty_switch
} // namespace esphome