#include "ActivityMonitor.h"

#include <Arduino.h>

#include <LittleFS.h>

void ActivityMonitor::begin() {
  pinMode(POWER_OFF_PIN, OUTPUT);

  File file = LittleFS.open(LOW_POWER_TIMEOUT_FILE, "r");
  if (file) {
    lowPowerTimeoutMs = file.parseInt();
    file.close();
  }

  file = LittleFS.open(POWER_OFF_TIMEOUT_FILE, "r");
  if (file) {
    powerOffTimeoutMs = file.parseInt();
    file.close();
  }
}

void ActivityMonitor::registerActivity() {
  lastActivityTimestamp = millis();
}

uint8_t ActivityMonitor::getLowPowerTimeoutMins() {
  return lowPowerTimeoutMs / 60 / 1000;
}

void ActivityMonitor::setLowPowerTimeoutMins(uint8_t minutes) {
  lowPowerTimeoutMs = minutes * 60 * 1000;
  File file = LittleFS.open(LOW_POWER_TIMEOUT_FILE, "w+");
  file.print(lowPowerTimeoutMs);
  file.close();
}

uint8_t ActivityMonitor::getPowerOffTimeoutMins() {
  return powerOffTimeoutMs / 60 / 1000;
}

void ActivityMonitor::setPowerOffTimeoutMins(uint8_t minutes) {
  powerOffTimeoutMs = minutes * 60 * 1000;
  File file = LittleFS.open(POWER_OFF_TIMEOUT_FILE, "w+");
  file.print(powerOffTimeoutMs);
  file.close();
}

void ActivityMonitor::onLowPowerMode(OnModeChangeCallback callback) {
  onLowPowerModeCallback = callback;
}

void ActivityMonitor::onDefaultMode(OnModeChangeCallback callback) {
  onDefaultModeCallback = callback;
}

void ActivityMonitor::run() {
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