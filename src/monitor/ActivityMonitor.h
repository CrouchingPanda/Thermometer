#ifndef ACTIVITY_MONITOR_H
#define ACTIVITY_MONITOR_H

#include <stdint.h>

#include <Arduino.h>

#include "../config/ConfigStore.h"

class UserActivityMonitor {

  ConfigStore& config;

  uint32_t lastActivityTimestamp = 0;

  // 0 = timeout disabled
  uint32_t lowPowerTimeoutMs;
  uint32_t powerOffTimeoutMs;

  enum Mode { DEFAULT, LOW_POWER } mode = DEFAULT;

  using OnModeChangeCallback = void(*)();
  OnModeChangeCallback onLowPowerModeCallback = nullptr;
  OnModeChangeCallback onDefaultModeCallback = nullptr;

  public:

  UserActivityMonitor(ConfigStore& config) : config(config) {};

  void begin();

  void registerActivity();

  uint8_t getLowPowerTimeoutMins();

  void setLowPowerTimeoutMins(uint8_t minutes);

  uint8_t getPowerOffTimeoutMins();

  void setPowerOffTimeoutMins(uint8_t minutes);

  void onLowPowerMode(OnModeChangeCallback callback);

  void onDefaultMode(OnModeChangeCallback callback);

  void run();
};

extern UserActivityMonitor ActivityMonitor;

#endif // !ACTIVITY_MONITOR_H
