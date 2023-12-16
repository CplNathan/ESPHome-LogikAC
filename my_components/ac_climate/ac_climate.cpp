#include "esphome/core/log.h"
#include "ac_climate.h"

namespace esphome
{
    namespace ac_climate
    {
        static IRsend *irsend;
        static IRMessages *irmessages;
        static const char *TAG = "ac_climate.climate";

        IRMessages *AC::get_messages()
        {
            return irmessages;
        }

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

            for (int i = 0; i < 8; i++)
            {
                while (!this->available())
                {
                }
                data[i] = this->read();
            }

            /*
                        while (data[0] != 0x0 && data[1] != 0x12)
                        {
                            while (!(this->available() >= 2))
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
            */

            uart::UARTDebug::log_binary(uart::UARTDirection::UART_DIRECTION_RX, data, ',');

            return data;
        }

        void AC::update_state()
        {
            std::vector<uint8_t> deviceData = read_state();

            bool isOff = deviceData[3] == 0x15 && deviceData[4] == 0x15;

            if (isOff)
            {
                this->fan_mode = climate::ClimateFanMode::CLIMATE_FAN_OFF;
                this->mode = climate::ClimateMode::CLIMATE_MODE_OFF;
            }
            else
            {
                bool isLow = (deviceData[3] == 0x15 && (deviceData[4] == 0x14 || deviceData[4] == 0x04));
                bool isSwing = deviceData[7] == 0x11;

                this->mode = climate::ClimateMode::CLIMATE_MODE_FAN_ONLY;
                this->fan_mode = isLow ? climate::ClimateFanMode::CLIMATE_FAN_LOW : climate::ClimateFanMode::CLIMATE_FAN_HIGH;
                this->swing_mode = isSwing ? climate::ClimateSwingMode::CLIMATE_SWING_VERTICAL : climate::ClimateSwingMode::CLIMATE_SWING_OFF;
            }
        }

        void AC::execute_update(uint32_t command)
        {
            irsend->sendNEC(command);
        }

        void AC::execute_updates(const std::vector<uint32_t> &commands)
        {
            for (auto iter = commands.begin(); iter < commands.end(); iter++)
            {
                this->execute_update(*iter);
                delay(5);
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
            this->publish_state();
        }

        void AC::control(const climate::ClimateCall &call)
        {
            auto currentModeValue = this->mode;
            auto callMode = call.get_mode();
            auto callModeValue = *callMode;
            bool togglePower = callMode.has_value() && currentModeValue != callModeValue ? (callModeValue == 0 || (callModeValue > 0 && currentModeValue == 0)) : false;

            auto currentFan = this->fan_mode;
            auto callFan = call.get_fan_mode();
            auto callFanValue = *callFan;
            bool toggleFan = callFan.has_value() && currentFan != callFanValue;

            auto currentSwing = this->swing_mode;
            auto callSwing = call.get_swing_mode();
            auto callSwingValue = *callSwing;
            bool toggleSwing = callSwing.has_value() && currentSwing != callSwing;

            std::vector<uint32_t> commands;

            if (togglePower)
            {
                commands.push_back(irmessages->togglePower);
            }

            if (toggleFan)
            {
                commands.push_back(irmessages->toggleFanSpeed);
            }

            if (toggleSwing)
            {
                commands.push_back(irmessages->toggleSwing);
            }

            this->execute_updates(commands);

            this->update_state();
            this->publish_state();
        }

        climate::ClimateTraits AC::traits()
        {
            climate::ClimateTraits traits;
            traits.add_supported_mode(climate::ClimateMode::CLIMATE_MODE_FAN_ONLY);
            traits.add_supported_mode(climate::ClimateMode::CLIMATE_MODE_OFF);
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
            ESP_LOGCONFIG(TAG, "AC climate dump");
        }

    } // namespace ac_climate
} // namespace esphome