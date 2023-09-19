#ifndef HEALTH_INDICATOR_H
#define HEALTH_INDICATOR_H

#include <Arduino.h>

class HealthIndicator {

  static constexpr uint16_t LED_ON_MILLIS = 100;
  static constexpr uint16_t LED_OFF_MILLIS = 1000;

  bool ledIsOn = false;
  uint32_t lastChangeTimestamp = 0;

  public:

  void begin();

  void run();
};

#endif  // !HEALTH_INDICATOR_H