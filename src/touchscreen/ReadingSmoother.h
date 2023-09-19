#ifndef READING_SMOOTHER_H
#define READING_SMOOTHER_H

#include <Arduino.h>

#include <EEPROM.h>

#include <Smoothed.h>

class ReadingSmoother {

  Smoothed<float>* smoother;

  uint8_t factor = 10;

  EEPROMClass* EEPROMInstance;
  uint16_t EEPROMStorageAddress;

  public:

  static constexpr uint8_t MAX_FACTOR = 100; // less smooth
  static constexpr uint8_t MIN_FACTOR = 1; // more smooth

  ReadingSmoother(EEPROMClass* EEPROM, uint16_t EEPROMAddress) 
    : EEPROMInstance(EEPROM), EEPROMStorageAddress(EEPROMAddress) {};

  void begin();

  uint8_t getFactor();

  void setFactor(uint8_t newFactor);

  void addReading(float reading);

  float smoothedValue();

  private:

  static uint8_t correctFactor(uint8_t factor);
};

#endif  // !READING_SMOOTHER_H
