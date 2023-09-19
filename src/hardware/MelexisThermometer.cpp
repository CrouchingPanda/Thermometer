#include "MelexisThermometer.h"

#include <Arduino.h>

#include <Wire.h>

void MelexisThermometer::begin(TwoWire* wire) {
  sensor = new Adafruit_MLX90614();
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, HIGH);
  delay(SENSOR_RESET_POWER_UP_TIME_MS);
  sensor->begin(MLX90614_I2CADDR, wire);
}

float MelexisThermometer::objectTemperatureFarengheit() {
  handlePendingChanges();
  if (sensorResetting) return cachedObjectTemperatureFarengheit;
  cachedObjectTemperatureFarengheit = sensor->readObjectTempF();
  return cachedObjectTemperatureFarengheit;
}

float MelexisThermometer::ambientTemperatureFarengheit() {
  handlePendingChanges();
  if (sensorResetting) return cachedAmbientTemperatureFarengheit;
  cachedAmbientTemperatureFarengheit = sensor->readAmbientTempF();
  return cachedAmbientTemperatureFarengheit;
}

float MelexisThermometer::getEmissivity() {
  handlePendingChanges();
  if (sensorResetting) return cachedEmissivity;
  cachedEmissivity = sensor->readEmissivity();
  return cachedEmissivity;
}

void MelexisThermometer::setEmissivity(float value) {
  if (sensorResetting) {
    emissivitySettingPending = true;
    pendingEmissivity = value;
  } else {
    writeEmissivityToSensor(value);
  }
}

void MelexisThermometer::handlePendingChanges() {
  if (sensorResetting) tryPowerUp();
  if (emissivitySettingPending) setPendingEmissivity();
}

void MelexisThermometer::setPendingEmissivity() {
  if (!emissivitySettingPending || sensorResetting) return;
  writeEmissivityToSensor(pendingEmissivity);
  emissivitySettingPending = false;
}

void MelexisThermometer::writeEmissivityToSensor(float value) {
  if (value > MAX_EMISSIVITY) {
    sensor->writeEmissivity(MAX_EMISSIVITY);
  } else if (value < MIN_EMISSIVITY) {
    sensor->writeEmissivity(MIN_EMISSIVITY);
  } else {
    sensor->writeEmissivity(value);
  }
  powerDown();
}

void MelexisThermometer::powerDown() {
  digitalWrite(SENSOR_POWER_PIN, LOW);
  sensorResetting = true;
  sensorPoweredDownTimestamp = millis();
  sensorPoweredUpTimestamp = sensorPoweredDownTimestamp + SENSOR_RESET_POWER_DOWN_TIME_MS;
}

void MelexisThermometer::tryPowerUp() {
  if (!sensorResetting) return;

  static bool isSensorOnPowerOnWait = false;

  if (
      millis() - sensorPoweredDownTimestamp >= SENSOR_RESET_POWER_DOWN_TIME_MS &&
      !isSensorOnPowerOnWait) {
    digitalWrite(SENSOR_POWER_PIN, HIGH);
    sensorPoweredUpTimestamp = millis();
    isSensorOnPowerOnWait = true;
  };

  if (millis() - sensorPoweredUpTimestamp >= SENSOR_RESET_POWER_UP_TIME_MS &&
      isSensorOnPowerOnWait) {
    sensorResetting = false;
    isSensorOnPowerOnWait = false;
  };
}