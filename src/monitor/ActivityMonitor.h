#ifndef ACTIVITY_MONITOR_H
#define ACTIVITY_MONITOR_H

#include <Arduino.h>

class ActivityMonitor {

  static constexpr uint8_t POWER_OFF_PIN = 6;

  static constexpr const char* LOW_POWER_TIMEOUT_FILE = "activity/low_power_timeout";
  static constexpr const char* POWER_OFF_TIMEOUT_FILE = "activity/power_off_timeout";

  uint32_t lastActivityTimestamp = 0;

  // 0 = timeout disabled
  uint32_t lowPowerTimeoutMs = 5 * 60 * 1000;
  uint32_t powerOffTimeoutMs = 7 * 60 * 1000;

  enum Mode { DEFAULT, LOW_POWER } mode = DEFAULT;

  using OnModeChangeCallback = void(*)();
  OnModeChangeCallback onLowPowerModeCallback = nullptr;
  OnModeChangeCallback onDefaultModeCallback = nullptr;

  public:

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

#endif // !ACTIVITY_MONITOR_H
