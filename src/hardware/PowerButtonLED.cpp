#include "PowerButtonLED.h"

#include <stdint.h>

#include <Arduino.h>

#include <jled.h>

#include "../config/ConfigStore.h"

constexpr const char* ON_BRIGHTNESS_CONFIG_KEY = "powerLedOnBrightness";
constexpr const char* MAX_BREATH_BRIGHTNESS_CONFIG_KEY = "powerLedMaxBreathBrightness";

constexpr uint8_t LED_PIN = 24;

constexpr uint16_t BREATHING_PERIOD_MS = 3000;

constexpr uint8_t DEFAUT_ON_BRIGHTNESS = 50;
constexpr uint8_t DEFAUT_MAX_BREATH_BRIGHTNESS = 130;

PowerButtonLED::PowerButtonLED(ConfigStore& config) :
  config(config),
  led(JLed(LED_PIN)) {};

void PowerButtonLED::begin() {
  onBrightness = config.getOrDefault(ON_BRIGHTNESS_CONFIG_KEY, DEFAUT_ON_BRIGHTNESS);
  maxBreathBrightness = config.getOrDefault(
    MAX_BREATH_BRIGHTNESS_CONFIG_KEY, 
    DEFAUT_MAX_BREATH_BRIGHTNESS
  );

  led.Set(onBrightness);
  led.Update();
}

void PowerButtonLED::on() {
  led.Update();
  if (state == ON || updatePending) return;
  led.MaxBrightness(255);
  led.Set(onBrightness);
  state = ON;
}

void PowerButtonLED::breathe() {
  led.Update();
  if (state == BREATHING || updatePending) return;
  led.MaxBrightness(maxBreathBrightness);
  led.Breathe(BREATHING_PERIOD_MS).Forever();
  state = BREATHING;
}

uint8_t PowerButtonLED::getOnBrightness() {
  return onBrightness;
}

void PowerButtonLED::setOnBrightness(uint8_t value) {
  if (value == onBrightness) return;
  onBrightness = value;
  config.set(ON_BRIGHTNESS_CONFIG_KEY, onBrightness);
  state = PENDING_UPDATE;
}

uint8_t PowerButtonLED::getMaxBreathBrightness() {
  return maxBreathBrightness;
}

void PowerButtonLED::setMaxBreathBrightness(uint8_t value) {
  if (value == maxBreathBrightness) return;
  maxBreathBrightness = value;
  config.set(MAX_BREATH_BRIGHTNESS_CONFIG_KEY, maxBreathBrightness);
  state = PENDING_UPDATE;
}

PowerButtonLED PowerLED = PowerButtonLED(Config);