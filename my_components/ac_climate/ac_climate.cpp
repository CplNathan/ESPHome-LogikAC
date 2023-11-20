#include "esphome/core/log.h"
#include "ac_climate.h"

namespace esphome
{
    namespace ac_climate
    {
        static IRsend *irsend;
        static IRMessages *irmessages;
        static const char *TAG = "ac_climate.climate";

        std::vector<uint8_t> AC::read_state()
        {
            int baud = this->parent_->get_baud_rate();
            int databits = this->parent_->get_data_bits();
            int us = 1 / baud * 1000000;

            // Try to clear out the buffer.
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

        void AC::update_state()
        {
            std::vector<uint8_t> deviceData = read_state();

            // Swing Off 0b00010010 (0x12),
            // Swing Off 0b10101000
            // Swing On  0b01001000 (0x48)
            // Swing On  0b00100100 (0x24)
            // (bool)(deviceData[5] & 0b00000010) || (bool)(deviceData[5] & 0b00100000);
            auto swingOff = deviceData[5] == 0x0;
            this->swing_mode = swingOff ? climate::ClimateSwingMode::CLIMATE_SWING_OFF : climate::ClimateSwingMode::CLIMATE_SWING_VERTICAL;

            switch (deviceData[3])
            {
            case 0x0:
                this->mode = climate::ClimateMode::CLIMATE_MODE_OFF;
                break;

            case 0x10:
                this->fan_mode = climate::ClimateFanMode::CLIMATE_FAN_LOW;
                this->mode = climate::ClimateMode::CLIMATE_MODE_FAN_ONLY;
                break;

            case 0x4:
                this->fan_mode = climate::ClimateFanMode::CLIMATE_FAN_HIGH;
                this->mode = climate::ClimateMode::CLIMATE_MODE_FAN_ONLY;
                break;
            }

            this->publish_state();
        }

        void AC::execute_updates(const std::vector<uint32_t> &commands)
        {
            for (auto iter = commands.begin(); iter < commands.end(); iter++)
            {
                irsend->sendNEC(*iter);
                delay(1);
            }
        }

        void AC::setup()
        {
            irsend = new IRsend(((InternalGPIOPin *)this->ir_pin_)->get_pin());
            irsend->begin();

            irmessages = new IRMessages(irsend);

            this->update_state();
        }

        void AC::update()
        {
            this->update_state();
        }

        // todo, string, fan speed etc
        // todo need to find out ir code for sweep, fan speed, ac mode, temp..?

        // Something less than 0x9 was temp up/down
        // 0x9 = mode select

        // Probably don't use the display lights as this requires for the screen to be awake for commands to work...
        // 0xB = display lights toggle (I think? if not just check again tomorrow, its somewhere around here.)

        void AC::control(const climate::ClimateCall &call)
        {
            bool togglePower = false;

            auto callMode = call.get_mode();
            auto currentMode = this->mode;
            if (currentMode != callMode)
            {
                togglePower = callMode == 0 || (callMode > 0 && currentMode == 0);
            }

            auto callFan = call.get_fan_mode();
            auto currentFan = this->fan_mode;

            // Only two modes for this so always a toggle if changed.
            bool toggleFan = currentFan != callFan;

            std::vector<uint32_t> commands;

            if (togglePower)
            {
                commands.push_back(irmessages->togglePower);
            }

            if (toggleFan)
            {
                commands.push_back(irmessages->toggleFanSpeed);
            }

            this->execute_updates(commands);

            this->update_state();
        }

        climate::ClimateTraits AC::traits()
        {
            climate::ClimateTraits traits;
            traits.add_supported_mode(climate::ClimateMode::CLIMATE_MODE_COOL);
            traits.add_supported_mode(climate::ClimateMode::CLIMATE_MODE_DRY);
            traits.add_supported_mode(climate::ClimateMode::CLIMATE_MODE_FAN_ONLY);
            traits.set_supports_current_temperature(false);
            traits.set_supports_two_point_target_temperature(false);
            traits.add_supported_fan_mode(climate::ClimateFanMode::CLIMATE_FAN_HIGH);
            traits.add_supported_fan_mode(climate::ClimateFanMode::CLIMATE_FAN_LOW);
            traits.add_supported_swing_mode(climate::ClimateSwingMode::CLIMATE_SWING_VERTICAL);
            traits.add_supported_swing_mode(climate::ClimateSwingMode::CLIMATE_SWING_OFF);

            return traits;
        }

        void AC::dump_config()
        {
            ESP_LOGCONFIG(TAG, "Empty custom switch");
        }

    } // namespace ac_climate
} // namespace esphome