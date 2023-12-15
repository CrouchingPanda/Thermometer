#ifndef HEALTH_MONITOR_H
#define HEALTH_MONITOR_H

#include <Arduino.h>

#include <NeoPixelConnect.h>

class HealthMonitor {

  static constexpr uint8_t LED_POWER_PIN = 16;
  static constexpr uint8_t LED_CONTROL_PIN = 17;
  static constexpr uint8_t LED_BRIGHTNESS = 50;

  static constexpr uint16_t LED_ON_MILLIS = 5;
  static constexpr uint16_t LED_OFF_MILLIS = 3000;

  NeoPixelConnect* led;

  bool ledIsOn = false;
  uint32_t lastChangeTimestamp = 0;

  public:

  void begin();

  void run();
};

#endif  // !HEALTH_MONITOR_H