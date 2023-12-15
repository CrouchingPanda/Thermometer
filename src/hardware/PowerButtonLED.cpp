#include "PowerButtonLED.h"

#include <Arduino.h>

#include <LittleFS.h>
#include <jled.h>

void PowerButtonLED::begin() {
  File file = LittleFS.open(ON_BRIGHTNESS_FILE, "r");
  if (file) {
    onBrightness = file.parseInt();
    file.close();
  }

  file = LittleFS.open(MAX_BREATH_BRIGHTNESS_FILE, "r");
  if (file) {
    maxBreathBrightness = file.parseInt();
    file.close();
  }

  led = new JLed(LED_PIN);
  led->Set(onBrightness);
  led->Update();
}

void PowerButtonLED::on() {
  led->Update();
  if (state == ON || updatePending) return;
  led->MaxBrightness(255);
  led->Set(onBrightness);
  state = ON;
}

void PowerButtonLED::breathe() {
  led->Update();
  if (state == BREATHING || updatePending) return;
  led->MaxBrightness(maxBreathBrightness);
  led->Breathe(BREATHING_PERIOD_MS).Forever();
  state = BREATHING;
}

uint8_t PowerButtonLED::getOnBrightness() {
  return onBrightness;
}

void PowerButtonLED::setOnBrightness(uint8_t value) {
  if (value == onBrightness) return;
  
  onBrightness = value;
  state = PENDING_UPDATE;
  
  File file = LittleFS.open(ON_BRIGHTNESS_FILE, "w+");
  if (file) {
    file.print(onBrightness);
    file.close();
  }
}

uint8_t PowerButtonLED::getMaxBreathBrightness() {
  return maxBreathBrightness;
}

void PowerButtonLED::setMaxBreathBrightness(uint8_t value) {
  if (value == maxBreathBrightness) return;
  
  maxBreathBrightness = value;
  state = PENDING_UPDATE;
  
  File file = LittleFS.open(MAX_BREATH_BRIGHTNESS_FILE, "w+");
  if (file) {
    file.print(maxBreathBrightness);
    file.close();
  }
}