#include "Battery.h"

#include <Arduino.h>

#include <Adafruit_LC709203F.h>

void Battery::begin(TwoWire* wire) {
  pinMode(VOLTAGE_PIN, OUTPUT);

  analogReadResolution(ADC_BITS);

  fuelGauge = new Adafruit_LC709203F();
  fuelGauge->begin(wire);
  fuelGauge->setPowerMode(LC709203F_POWER_OPERATE);
  fuelGauge->setPackAPA(BATTERY_APA);
  fuelGauge->setBattProfile(BATTERY_PROFILE);
  fuelGauge->setTemperatureMode(LC709203F_TEMPERATURE_THERMISTOR);
  fuelGauge->setThermistorB(BATTERY_THERMISTOR_B_VALUE);
}

bool Battery::isCharging() {
  float vccPinVoltage = getVoltageReading(CHARGING_STATUS_CHECK_PIN);
  return vccPinVoltage >= MIN_CHARGING_VOLTAGE_MV;
}

float Battery::getVoltageReading(uint8_t pin) {
  digitalWrite(VOLTAGE_PIN, HIGH);

  uint16_t adcFraction = analogRead(CHARGING_STATUS_CHECK_PIN);
  float reading = adcFractionToVoltage(adcFraction) * VOLTAGE_DIVIDER_RATIO;
  
  digitalWrite(VOLTAGE_PIN, LOW);
  return reading;
}

float Battery::adcFractionToVoltage(uint16_t adc) {
  static float factor = REF_BOARD_VOLTAGE / (1u << ADC_BITS);
  return adc * factor;
}

uint8_t Battery::chargePercentage() {
  return fuelGauge->cellPercent();
}

float Battery::temperatureFahrenheit() {
  return fuelGauge->getCellTemperature() * 9.0 / 5.0 + 32.0;
}
