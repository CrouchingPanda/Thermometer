#include <math.h>

#include "Thermistor.h"

#include <Arduino.h>

#include <lvgl.h>

#include "ADC.h"

void Thermistor::begin() {
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);

  adc = new ADC(SENSE_PIN);
  adc->begin();
}

float Thermistor::temperatureFarengheit() {
  digitalWrite(POWER_PIN, HIGH);

  float thermistorResistance = SERIES_RESISTANCE / ((1 / adc->reading()) - 1);
  float absoluteTemperature = 1.0 / (
    A_COEFFICIENT + 
    B_COEFFICIENT * log(thermistorResistance) + 
    C_COEFFICIENT * pow(log(thermistorResistance), 3)
  );
  float farengheitTemperature = ((absoluteTemperature - 273.15) * 9.0) / 5.0 + 32.0;

  digitalWrite(POWER_PIN, LOW);

  return farengheitTemperature;
}
