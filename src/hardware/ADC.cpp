#include "ADC.h"

#include <Arduino.h>

void ADC::begin() {
  analogReadResolution(BIT_RESOLUTION);
  pinMode(pin, INPUT);
}

float ADC::reading() {
  static uint16_t divisionCount = 1u << BIT_RESOLUTION;
  return (float) analogRead(pin) / divisionCount;
}
