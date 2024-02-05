#include "HealthMonitor.h"

#include <stdint.h>

#include <Arduino.h>

#include <NeoPixelConnect.h>

constexpr uint8_t LED_POWER_PIN = 16;
constexpr uint8_t LED_CONTROL_PIN = 17;
constexpr uint8_t LED_BRIGHTNESS = 50;

constexpr uint8_t LED_ON_MILLIS = 5;
constexpr uint16_t LED_OFF_MILLIS = 3000;

SystemHealthMonitor::SystemHealthMonitor() : 
  led(
    NeoPixelConnect(
      LED_CONTROL_PIN,
      1, // one LED
      pio1,
      0 // first state machine
    )
  ) {};

void SystemHealthMonitor::begin() {
  pinMode(LED_POWER_PIN, OUTPUT);
  digitalWrite(LED_POWER_PIN, HIGH);
  led.neoPixelClear();
}

void SystemHealthMonitor::run() {
  uint32_t timestamp = millis();
  
  if (ledIsOn && timestamp - lastChangeTimestamp > LED_ON_MILLIS) {
    ledIsOn = false;
    lastChangeTimestamp = timestamp;
    led.neoPixelClear();
  } else if (!ledIsOn && timestamp - lastChangeTimestamp > LED_OFF_MILLIS) {
    ledIsOn = true;
    lastChangeTimestamp = timestamp;
    led.neoPixelFill(LED_BRIGHTNESS, 0, 0);
  }
}

SystemHealthMonitor HealthMonitor = SystemHealthMonitor();