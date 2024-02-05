#ifndef MELEXIS_THERMOMETER
#define MELEXIS_THERMOMETER

#include <stdint.h>

#include <Arduino.h>

#include <Smoothed.h>
#include <Adafruit_MLX90614.h>

#include "../config/ConfigStore.h"

class MelexisThermometer {

  ConfigStore& config;

  Smoothed<double> smoother;
  
  uint8_t smoothingFactor; // 1..MAX_SMOOTHING_FACTOR; more = less smooth

  Adafruit_MLX90614 sensor;

  float emissivity;
  
  double roomTemperatureOffsetCelsius;

  public:

  MelexisThermometer(ConfigStore& config);

  void begin();

  float objectTemperatureFahrenheit();

  float onDieTemperatureFahrenheit();
  
  float roomTemperatureFahrenheit();

  float getEmissivity();

  void setEmissivity(float value);

  float getRoomTemperatureOffsetCelsius();
  
  void setRoomTemperatureOffsetCelsius(float offset);

  uint8_t getObjectTemperatureSmoothingFactor();

  void setObjectTemperatureSmoothingFactor(uint8_t value);
};

extern MelexisThermometer Thermometer;

#endif  // !MELEXIS_THERMOMETER