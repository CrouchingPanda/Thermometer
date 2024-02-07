#include "Battery.h"

#include <stdint.h>

#include <Arduino.h>

#include <Wire.h>

#include <Adafruit_LC709203F.h>

constexpr uint16_t ADC_BITS = 12;

// we always read 1/2 of battery voltage because full (4.2V) is too much
// for the analog pin
constexpr float VOLTAGE_DIVIDER_RATIO = 2.0;
constexpr float MIN_CHARGING_VOLTAGE_MV = 4.5;
constexpr float REF_BOARD_VOLTAGE = 3.3;

constexpr uint8_t CHARGING_STATUS_CHECK_PIN = A2;
constexpr uint8_t VOLTAGE_PIN = 14;

// per LC709203F datasheet for 500mAh 3.7V battery
constexpr uint8_t BATTERY_APA = 0x10;
constexpr uint8_t BATTERY_PROFILE = 1;

// B25/B50 for thermistor P/N NXFT15XH103FEAB050
constexpr uint16_t BATTERY_THERMISTOR_B_VALUE = 3380;

void Battery::begin() {
  pinMode(VOLTAGE_PIN, OUTPUT);

  analogReadResolution(ADC_BITS);

  fuelGauge.begin(&Wire1);
  fuelGauge.setPowerMode(LC709203F_POWER_OPERATE);
  fuelGauge.setPackAPA(BATTERY_APA);
  fuelGauge.setBattProfile(BATTERY_PROFILE);
  fuelGauge.setTemperatureMode(LC709203F_TEMPERATURE_THERMISTOR);
  fuelGauge.setThermistorB(BATTERY_THERMISTOR_B_VALUE);
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
  return fuelGauge.cellPercent();
}

float Battery::temperatureFahrenheit() {
  return fuelGauge.getCellTemperature() * 9.0 / 5.0 + 32.0;
}

Battery LiPoBattery = Battery();
