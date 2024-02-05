#ifndef HEALTH_MONITOR_H
#define HEALTH_MONITOR_H

#include <stdint.h>

#include <Arduino.h>

#include <NeoPixelConnect.h>

class SystemHealthMonitor {

  NeoPixelConnect led;

  bool ledIsOn = false;
  uint32_t lastChangeTimestamp = 0;

  public:

  SystemHealthMonitor();

  void begin();

  void run();
};

extern SystemHealthMonitor HealthMonitor;

#endif  // !HEALTH_MONITOR_H