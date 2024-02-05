#ifndef CALIBRATION_BOARD_RECEIVER_H
#define CALIBRATION_BOARD_RECEIVER_H

#include <stdint.h>

#include <Arduino.h>

#include <jled.h>
#include <SerialTransfer.h>

class CalibrationBoardReceiver {

  uint32_t lastCommTimestamp;

  float primaryThermistorTempF = 0.0;
  float secondaryThermistorTempF = 0.0;

  SerialTransfer receiver;

  JLed receiveLed;
  JLed connectionLed;

  public:

  CalibrationBoardReceiver();

  void begin();

  void run();

  bool isConnected();

  float primaryThermistorTemperatureFahrenheit();
  
  float secondaryThermistorTemperatureFahrenheit();
};

extern CalibrationBoardReceiver CalibrationReceiver;

#endif // !CALIBRATION_BOARD_RECEIVER_H