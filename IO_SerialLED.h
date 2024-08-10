/*
 *  © 2024, STRV All rights reserved.
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef IO_SERIALLED_H
#define IO_SERIALLED_H

#ifdef ARDUINO_ARCH_ESP32

#include "driver/rmt.h"
#include "IODevice.h"
#include "TrackManager.h"

// 0 to 3 is used for DCC wave form generation
#define IO_SERIALLED_RMT_CH (7)

class SerialLED : public IODevice {
private:
  static constexpr int BITS_PER_LED = 24;
  static constexpr unsigned long ALERT_KEEP_DURATION = 5*1000*1000;

  int _pin = -1;
  rmt_config_t _rmt_config;
  rmt_item32_t _led_data[BITS_PER_LED];
  POWERMODE _power_prev = POWERMODE::ALERT;
  unsigned long _alert_trig_us = 0;

  void setColor(const uint8_t* grb)
  {
    int cnt = 0;
    for (int c = 0; c < 3; ++c)
    {
      for (int i = 7; i >= 0; --i)
      {
        if ((grb[c] >> i) & 0x01)
        {
          _led_data[cnt].level0 = 1;
          _led_data[cnt].duration0 = 6;
          _led_data[cnt].level1 = 0;
          _led_data[cnt].duration1 = 6;
        }
        else
        {
          _led_data[cnt].level0 = 1;
          _led_data[cnt].duration0 = 3;
          _led_data[cnt].level1 = 0;
          _led_data[cnt].duration1 = 9;
        }
        ++cnt;
      }
    }
    rmt_write_items(_rmt_config.channel, _led_data, BITS_PER_LED, 0);
  }

  void setColor(const uint8_t r, const uint8_t g, const uint8_t b)
  {
    uint8_t grb[3] = {g, r, b};
    setColor(grb);
  }

public:
  static void create(const uint8_t pin) {
    new SerialLED(pin);
  }

protected:
  SerialLED(const uint8_t pin)
  : _pin(pin)
  {
    bzero(&_rmt_config, sizeof(rmt_config_t));
    _rmt_config.channel = (rmt_channel_t)IO_SERIALLED_RMT_CH;
    _rmt_config.clk_div = 8;
    _rmt_config.gpio_num = (gpio_num_t)_pin;
    _rmt_config.mem_block_num = 1;
    _rmt_config.rmt_mode = RMT_MODE_TX;
    ESP_ERROR_CHECK(rmt_config(&_rmt_config));
    ESP_ERROR_CHECK(rmt_driver_install(_rmt_config.channel, 0, 0));
    setColor(0,0,0);
    addDevice(this);
  }

  void _loop(unsigned long currentMicros) override {
    const auto md = TrackManager::getMainDrivers();
    POWERMODE power = md[0]->getPower();
    if (power == POWERMODE::ALERT)
      _alert_trig_us = currentMicros;
    else if (power == POWERMODE::ON && _power_prev == POWERMODE::ALERT)
    {
      if (currentMicros - _alert_trig_us < ALERT_KEEP_DURATION)
        power = _power_prev;
    }

    switch (power)
    {
      case POWERMODE::OFF:
      setColor(32,32,32);
      break;

      case POWERMODE::ON:
      setColor(0,128,0);
      break;

      case POWERMODE::ALERT:
      setColor(64,64,0);
      break;

      case POWERMODE::OVERLOAD:
      setColor(128,0,0);
      break;
    }
    _power_prev = power;
    delayUntil(currentMicros + 10000UL);
  }

  void _display() override {
    DIAG(F("SerialLED Configured."));
  }
};

#endif
#endif