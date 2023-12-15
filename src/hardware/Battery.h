#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>

#include <Adafruit_LC709203F.h>

class Battery {

  static constexpr uint16_t ADC_BITS = 12;

  // we always read 1/2 of battery voltage because full (4.2V) is too much
  // for the analog pin
  static constexpr float VOLTAGE_DIVIDER_RATIO = 2.0;
  static constexpr float MIN_CHARGING_VOLTAGE_MV = 4.5;
  static constexpr float REF_BOARD_VOLTAGE = 3.3;

  static constexpr uint8_t CHARGING_STATUS_CHECK_PIN = A2;
  static constexpr uint8_t VOLTAGE_PIN = 14;

  // battery-specific per LC709203F datasheet
  static constexpr uint8_t BATTERY_APA = 0x10;
  static constexpr uint8_t BATTERY_PROFILE = 1;

  // thermistor-specific
  static constexpr uint16_t BATTERY_THERMISTOR_B_VALUE = 3936;

  Adafruit_LC709203F* fuelGauge;

  public:
  
  void begin(TwoWire* wire);

  bool isCharging();

  uint8_t chargePercentage();

  float temperatureFahrenheit();

  private:
  
  float getVoltageReading(uint8_t pin);

  float adcFractionToVoltage(uint16_t adc);
};

#endif  // !BATTERY_H
