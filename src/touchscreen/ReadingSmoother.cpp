#include "ReadingSmoother.h"

#include <Arduino.h>

#include <EEPROM.h>

#include <Smoothed.h>

void ReadingSmoother::begin() {
  smoother = new Smoothed<float>();
  factor = EEPROMInstance->read(EEPROMStorageAddress);
  smoother->begin(SMOOTHED_EXPONENTIAL, correctFactor(factor));
}

uint8_t ReadingSmoother::getFactor() {
  return factor;
}

void ReadingSmoother::setFactor(uint8_t newFactor) {
  if (factor == newFactor) return;
  newFactor = correctFactor(newFactor);

  delete smoother;
  smoother = new Smoothed<float>();
  smoother->begin(SMOOTHED_EXPONENTIAL, newFactor);

  EEPROMInstance->write(EEPROMStorageAddress, newFactor);
  EEPROMInstance->commit();
}

void ReadingSmoother::addReading(float reading) {
  smoother->add(reading);
}

float ReadingSmoother::smoothedValue() {
  return smoother->get();
}

uint8_t ReadingSmoother::correctFactor(uint8_t factor) {
  if (factor > MAX_FACTOR) return MAX_FACTOR;
  if (factor < MIN_FACTOR) return MIN_FACTOR;
  return factor;
}
