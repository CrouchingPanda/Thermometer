#include "ActivityMonitor.h"

#include <stdint.h>

#include <Arduino.h>

#include "../config/ConfigStore.h"

constexpr uint8_t POWER_OFF_PIN = 6;

constexpr const char* LOW_POWER_TIMEOUT_CONFIG_KEY = "activityLowPowerTimeout";
constexpr uint32_t DEFAULT_LOW_POWER_TIMEOUT_MS = 5 * 60 * 1000;

constexpr const char* POWER_OFF_TIMEOUT_CONFIG_KEY = "activityPowerOffTimeout";
constexpr uint32_t DEFAULT_POWER_OFF_TIMEOUT_MS = 7 * 60 * 1000;

void UserActivityMonitor::begin() {
  pinMode(POWER_OFF_PIN, OUTPUT);

  lowPowerTimeoutMs = config.getOrDefault(
    LOW_POWER_TIMEOUT_CONFIG_KEY,
    DEFAULT_LOW_POWER_TIMEOUT_MS
  );

  powerOffTimeoutMs = config.getOrDefault(
    POWER_OFF_TIMEOUT_CONFIG_KEY,
    DEFAULT_POWER_OFF_TIMEOUT_MS
  );
}

void UserActivityMonitor::registerActivity() {
  lastActivityTimestamp = millis();
}

uint8_t UserActivityMonitor::getLowPowerTimeoutMins() {
  return lowPowerTimeoutMs / 60 / 1000;
}

void UserActivityMonitor::setLowPowerTimeoutMins(uint8_t minutes) {
  lowPowerTimeoutMs = minutes * 60 * 1000;
  config.set(LOW_POWER_TIMEOUT_CONFIG_KEY, lowPowerTimeoutMs);
}

uint8_t UserActivityMonitor::getPowerOffTimeoutMins() {
  return powerOffTimeoutMs / 60 / 1000;
}

void UserActivityMonitor::setPowerOffTimeoutMins(uint8_t minutes) {
  powerOffTimeoutMs = minutes * 60 * 1000;
  config.set(POWER_OFF_TIMEOUT_CONFIG_KEY, powerOffTimeoutMs);
}

void UserActivityMonitor::onLowPowerMode(OnModeChangeCallback callback) {
  onLowPowerModeCallback = callback;
}

void UserActivityMonitor::onDefaultMode(OnModeChangeCallback callback) {
  onDefaultModeCallback = callback;
}

void UserActivityMonitor::run() {
  uint32_t inactivityMillis = millis() - lastActivityTimestamp;
  
  if (powerOffTimeoutMs > 0 && inactivityMillis >= powerOffTimeoutMs) {
    digitalWrite(POWER_OFF_PIN, HIGH);
    digitalWrite(POWER_OFF_PIN, LOW);
    return;
  } 
  
  if (lowPowerTimeoutMs > 0 && inactivityMillis >= lowPowerTimeoutMs && mode != LOW_POWER) {
    mode = LOW_POWER;
    if (onLowPowerModeCallback != nullptr) onLowPowerModeCallback();
  } else if (inactivityMillis < lowPowerTimeoutMs && mode != DEFAULT) {
    mode = DEFAULT;
    if (onDefaultModeCallback != nullptr) onDefaultModeCallback();
  }
}

UserActivityMonitor ActivityMonitor = UserActivityMonitor(Config);