#include <math.h>

#include "MelexisThermometer.h"

#include <Arduino.h>

#include <Wire.h>
#include <LittleFS.h>

#include <Smoothed.h>

void MelexisThermometer::begin(TwoWire* wire) {
  sensor = new Adafruit_MLX90614();
  sensor->begin(MLX90614_I2CADDR, wire);

  File file = LittleFS.open(EMISSIVITY_FILE, "r");
  if (file) {
    emissivity = file.parseFloat();
    file.close();
  }

  file = LittleFS.open(AMBIENT_CORRECTION_OFFSET_FILE, "r");
  if (file) {
    roomTemperatureOffsetCelsius = file.parseFloat();
    file.close();
  }

  smoother = new Smoothed<float>();
  file = LittleFS.open(SMOOTHING_FACTOR_FILE, "r");
  if (file) {
    smoothingFactor = file.parseInt();
    file.close();
  }
  smoother->begin(SMOOTHED_EXPONENTIAL, smoothingFactor);
}

float MelexisThermometer::objectTemperatureFahrenheit() {
  float quadRoom = pow(sensor->readAmbientTempC() - roomTemperatureOffsetCelsius + 273.15, 4);
  float quadObject = pow(sensor->readObjectTempC() + 273.15, 4);
  float correctedObject = pow((quadObject - quadRoom + emissivity * quadRoom) / emissivity, 0.25);
  float correctedObjectFahrenheit = (correctedObject - 273.15) * 1.8 + 32.0;
  smoother->add(correctedObjectFahrenheit);
  return smoother->get();
}

float MelexisThermometer::onDieTemperatureFahrenheit() {
  return sensor->readAmbientTempF();
}

float MelexisThermometer::roomTemperatureFahrenheit() {
  return (sensor->readAmbientTempC() - roomTemperatureOffsetCelsius) * 1.8 + 32.0;
}

float MelexisThermometer::getEmissivity() {
  return emissivity;
}

void MelexisThermometer::setEmissivity(float value) {
  if (emissivity == value) return;
  
  if (value > MAX_EMISSIVITY) {
    emissivity = MAX_EMISSIVITY;
  } else if (value < MIN_EMISSIVITY) {
    emissivity = MIN_EMISSIVITY;
  } else {
    emissivity = value;
  }

  File file = LittleFS.open(EMISSIVITY_FILE, "w+");
  if (file) {
    file.print(emissivity);
    file.close();
  }
}

float MelexisThermometer::getRoomTemperatureOffsetCelsius() {
  return roomTemperatureOffsetCelsius;
}

void MelexisThermometer::setRoomTemperatureOffsetCelsius(float offset) {
  if (offset == roomTemperatureOffsetCelsius) return;
  roomTemperatureOffsetCelsius = offset;
  File file = LittleFS.open(AMBIENT_CORRECTION_OFFSET_FILE, "w+");
  if (file) {
    file.print(roomTemperatureOffsetCelsius);
    file.close();
  }
}

uint8_t MelexisThermometer::getObjectTemperatureSmoothingFactor() {
  return smoothingFactor;
}

void MelexisThermometer::setObjectTemperatureSmoothingFactor(uint8_t value) {
  if (smoothingFactor == value) return;
  smoothingFactor = (value == 0 || value > MAX_SMOOTHING_FACTOR) 
    ? MAX_SMOOTHING_FACTOR 
    : value;

  delete smoother;
  smoother = new Smoothed<float>();
  smoother->begin(SMOOTHED_EXPONENTIAL, smoothingFactor);

  File file = LittleFS.open(SMOOTHING_FACTOR_FILE, "w+");
  file.print(smoothingFactor);
  file.close();
}
