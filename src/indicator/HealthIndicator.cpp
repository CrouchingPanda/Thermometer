#include "HealthIndicator.h"

#include <Arduino.h>

void HealthIndicator::begin() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void HealthIndicator::run() {
  uint32_t timestamp = millis();
  
  if (ledIsOn && timestamp - lastChangeTimestamp > LED_ON_MILLIS) {
    ledIsOn = false;
    lastChangeTimestamp = timestamp;
  } else if (!ledIsOn && timestamp - lastChangeTimestamp > LED_OFF_MILLIS) {
    ledIsOn = true;
    lastChangeTimestamp = timestamp;
  }

  digitalWrite(LED_BUILTIN, ledIsOn);
}