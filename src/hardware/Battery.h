#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

#include <Arduino.h>

#include <Adafruit_LC709203F.h>

class Battery {

  Adafruit_LC709203F fuelGauge;

  public:
  
  Battery() : fuelGauge(Adafruit_LC709203F()) {};

  void begin();

  bool isCharging();

  uint8_t chargePercentage();

  float temperatureFahrenheit();

  private:
  
  float getVoltageReading(uint8_t pin);

  float adcFractionToVoltage(uint16_t adc);
};

extern Battery LiPoBattery;

#endif  // !BATTERY_H
