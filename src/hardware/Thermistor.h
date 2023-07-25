#ifndef THERMISTOR_H
#define THERMISTOR_H

#include <Arduino.h>

#include "ADC.h"

class Thermistor {

  static constexpr uint8_t POWER_PIN = 25;
  static constexpr uint8_t SENSE_PIN = A1;

  static constexpr float SERIES_RESISTANCE = 10000.0;

  // Steinhart - Hart equation coefficients for USP10982 thermistor
  // calculated with 0°C, 25°C and 60°C points per RT table
  static constexpr float A_COEFFICIENT = 1.127598029e-3;
  static constexpr float B_COEFFICIENT = 2.343604016e-4;
  static constexpr float C_COEFFICIENT = 8.687814791e-8;

  ADC* adc;

  public:

  void begin();

  float temperatureFarengheit();

};

#endif // !THERMISTOR_H
