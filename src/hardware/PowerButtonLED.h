#ifndef POWER_BUTTON_LED_H
#define POWER_BUTTON_LED_H

#include <Arduino.h>

#include <jled.h>

class PowerButtonLED {

  static constexpr uint8_t LED_PIN = 24;

  static constexpr const char* ON_BRIGHTNESS_FILE = "power_led/on_brightness";
  static constexpr const char* MAX_BREATH_BRIGHTNESS_FILE = "power_led/max_breath_brightness";

  static constexpr uint16_t BREATHING_PERIOD_MS = 3000;

  enum State { ON, BREATHING, PENDING_UPDATE } state = ON;

  JLed* led;

  uint8_t onBrightness = 50;
  uint8_t maxBreathBrightness = 127;

  bool updatePending = false;

  public:

  void begin();

  void breathe();

  void on();

  uint8_t getOnBrightness();

  void setOnBrightness(uint8_t value);

  uint8_t getMaxBreathBrightness();

  void setMaxBreathBrightness(uint8_t value);
};

#endif // !POWER_BUTTON_LED_H
