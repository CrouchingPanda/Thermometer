#include "HealthMonitor.h"

#include <Arduino.h>

#include <NeoPixelConnect.h>

void HealthMonitor::begin() {
  pinMode(LED_POWER_PIN, OUTPUT);
  digitalWrite(LED_POWER_PIN, HIGH);

  led = new NeoPixelConnect(
    LED_CONTROL_PIN,
    1, // one LED
    pio1,
    0 // first state machine
  );
  led->neoPixelClear();
}

void HealthMonitor::run() {
  uint32_t timestamp = millis();
  
  if (ledIsOn && timestamp - lastChangeTimestamp > LED_ON_MILLIS) {
    ledIsOn = false;
    lastChangeTimestamp = timestamp;
    led->neoPixelClear();
  } else if (!ledIsOn && timestamp - lastChangeTimestamp > LED_OFF_MILLIS) {
    ledIsOn = true;
    lastChangeTimestamp = timestamp;
    led->neoPixelFill(LED_BRIGHTNESS, 0, 0);
  }
}