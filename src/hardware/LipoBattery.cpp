#include "LipoBattery.h"

#include <Arduino.h>

#include <Adafruit_LC709203F.h>
#include <Adc.h>

void LipoBattery::begin(TwoWire* wire) {
  pinMode(CHECK_GATE_PIN, OUTPUT);

  adc = new Adc(CHARGING_STATUS_CHECK_PIN);
  adc->begin();

  fuelGauge = new Adafruit_LC709203F();
  fuelGauge->begin(wire);
  fuelGauge->setPackSize(BATTERY_CAPACITY);
}

bool LipoBattery::isCharging() {
  float vccPinVoltage = getVoltageReading(CHARGING_STATUS_CHECK_PIN);
  return vccPinVoltage >= MIN_CHARGING_VOLTAGE_MV;
}

float LipoBattery::getVoltageReading(uint8_t pin) {
  digitalWrite(CHECK_GATE_PIN, HIGH);
  float reading = adc->reading() * Adc::REF_BOARD_VOLTAGE * VOLTAGE_DIVIDER_RATIO;
  digitalWrite(CHECK_GATE_PIN, LOW);
  return reading;
}

uint8_t LipoBattery::chargePercentage() {
  fuelGauge->setPowerMode(LC709203F_POWER_OPERATE);
  fuelGauge->initRSOC();
  uint8_t chargePercentage = fuelGauge->cellPercent();
  fuelGauge->setPowerMode(LC709203F_POWER_SLEEP);
  return chargePercentage;
}