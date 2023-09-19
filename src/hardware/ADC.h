#ifndef ADC_H
#define ADC_H

#include <Arduino.h>

class ADC {

  static constexpr uint8_t BIT_RESOLUTION = 12;

  uint8_t pin;

  public:

  static constexpr float REF_BOARD_VOLTAGE = 3.3;

  ADC(uint8_t pin) : pin(pin) {};

  void begin();

  float reading();
};

#endif // !ADC_H
