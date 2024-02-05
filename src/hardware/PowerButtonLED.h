#ifndef POWER_BUTTON_LED_H
#define POWER_BUTTON_LED_H

#include <stdint.h>

#include <Arduino.h>

#include <jled.h>

#include "../config/ConfigStore.h"

class PowerButtonLED {

  ConfigStore& config;

  JLed led;

  enum State { ON, BREATHING, PENDING_UPDATE } state = ON;

  uint8_t onBrightness;
  uint8_t maxBreathBrightness;

  bool updatePending = false;

  public:

  PowerButtonLED(ConfigStore& config);

  void begin();

  void breathe();

  void on();

  uint8_t getOnBrightness();

  void setOnBrightness(uint8_t value);

  uint8_t getMaxBreathBrightness();

  void setMaxBreathBrightness(uint8_t value);
};

extern PowerButtonLED PowerLED;

#endif // !POWER_BUTTON_LED_H
