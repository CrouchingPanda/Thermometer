#ifndef LIPO_BATTERY_H
#define LIPO_BATTERY_H

#include <Arduino.h>

#include <Adafruit_LC709203F.h>
#include <Adc.h>

class LipoBattery {

  static constexpr float MIN_CHARGING_VOLTAGE_MV = 4.5;

  // we always read 1/2 of battery voltage because full (4.2V) is too much
  // for the analog pin
  static constexpr float VOLTAGE_DIVIDER_RATIO = 2.0;

  static constexpr uint8_t CHARGING_STATUS_CHECK_PIN = A0;
  static constexpr uint8_t CHECK_GATE_PIN = 24;

  static constexpr lc709203_adjustment_t BATTERY_CAPACITY = LC709203F_APA_500MAH;

  Adafruit_LC709203F* fuelGauge;

  Adc* adc;

  public:
  
  void begin(TwoWire* wire);

  bool isCharging();

  uint8_t chargePercentage();

  private:
  
  float getVoltageReading(uint8_t pin);
};

#endif  // !LIPO_BATTERY_H
