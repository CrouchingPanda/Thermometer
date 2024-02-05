#include "MelexisThermometer.h"

#include <math.h>
#include <stdint.h>

#include <Arduino.h>

#include <Wire.h>

#include <Smoothed.h>

#include "../config/ConfigStore.h"

constexpr const char* EMISSIVITY_CONFIG_KEY = "thermometerEmissivity";
constexpr float MAX_EMISSIVITY = 1.0;
constexpr float MIN_EMISSIVITY = 0.1;

constexpr const char* AMBIENT_CORRECTION_OFFSET_CONFIG_KEY = "thermometerAmbientCorrection";

// see "Temperature reading dependence on VDD" chapter in MLX90614 datasheet
constexpr double DEFAUT_TEMPERATURE_OFFSET_CELCIUS = (3.3 - 3.0) * 0.6;

constexpr const char* SMOOTHING_FACTOR_CONFIG_KEY = "thermometerSmoothingFactor";
constexpr uint8_t MAX_SMOOTHING_FACTOR = 100;
constexpr uint8_t DEFAULT_SMOOTHING_FACTOR = 10;

MelexisThermometer::MelexisThermometer(ConfigStore& config) :
  config(config),
  sensor(Adafruit_MLX90614()),
  smoother(Smoothed<double>()) {};

void MelexisThermometer::begin() {
  sensor.begin(MLX90614_I2CADDR, &Wire1);

  emissivity = config.getOrDefault(EMISSIVITY_CONFIG_KEY, MAX_EMISSIVITY);
  
  roomTemperatureOffsetCelsius = config.getOrDefault(
    AMBIENT_CORRECTION_OFFSET_CONFIG_KEY, 
    DEFAUT_TEMPERATURE_OFFSET_CELCIUS
  );
  
  smoothingFactor = config.getOrDefault(
    SMOOTHING_FACTOR_CONFIG_KEY, 
    DEFAULT_SMOOTHING_FACTOR
  );
  smoother.begin(SMOOTHED_EXPONENTIAL, smoothingFactor);
}

// see README for the algorithm explanation
float MelexisThermometer::objectTemperatureFahrenheit() {
  double quadRoom = pow(sensor.readAmbientTempC() - roomTemperatureOffsetCelsius + 273.15, 4);
  double quadObject = pow(sensor.readObjectTempC() + 273.15, 4);
  double correctedObject = pow((quadObject - quadRoom + emissivity * quadRoom) / emissivity, 0.25);
  double correctedObjectFahrenheit = (correctedObject - 273.15) * 1.8 + 32.0;
  smoother.add(correctedObjectFahrenheit);
  return smoother.get();
}

float MelexisThermometer::onDieTemperatureFahrenheit() {
  return sensor.readAmbientTempF();
}

float MelexisThermometer::roomTemperatureFahrenheit() {
  return (sensor.readAmbientTempC() - roomTemperatureOffsetCelsius) * 1.8 + 32.0;
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

  config.set(EMISSIVITY_CONFIG_KEY, emissivity);
}

float MelexisThermometer::getRoomTemperatureOffsetCelsius() {
  return roomTemperatureOffsetCelsius;
}

void MelexisThermometer::setRoomTemperatureOffsetCelsius(float offset) {
  if (offset == roomTemperatureOffsetCelsius) return;
  roomTemperatureOffsetCelsius = offset;
  config.set(AMBIENT_CORRECTION_OFFSET_CONFIG_KEY, roomTemperatureOffsetCelsius);
}

uint8_t MelexisThermometer::getObjectTemperatureSmoothingFactor() {
  return smoothingFactor;
}

void MelexisThermometer::setObjectTemperatureSmoothingFactor(uint8_t value) {
  if (smoothingFactor == value) return;
  smoothingFactor = (value == 0 || value > MAX_SMOOTHING_FACTOR) 
    ? MAX_SMOOTHING_FACTOR 
    : value;

  smoother = Smoothed<double>();
  smoother.begin(SMOOTHED_EXPONENTIAL, smoothingFactor);
  config.set(SMOOTHING_FACTOR_CONFIG_KEY, smoothingFactor);
}

MelexisThermometer Thermometer = MelexisThermometer(Config);