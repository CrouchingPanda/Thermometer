#ifndef CALIBRATION_BOARD_RECEIVER_H
#define CALIBRATION_BOARD_RECEIVER_H

#include <Arduino.h>

#include <jled.h>
#include <SerialTransfer.h>

class CalibrationBoardReceiver {

  static constexpr uint8_t CONNECTION_LED_PIN = 11;
  static constexpr uint8_t CONNECTION_LED_BRIGHTNESS = 50;

  static constexpr uint8_t RECEIVE_LED_PIN = 10;
  static constexpr uint8_t RECEIVE_LED_BLINK_ON_MS = 10;

  static constexpr uint8_t SERIAL_RX_PIN = 9;
  static constexpr uint8_t SERIAL_TX_PIN = 8;
  static constexpr uint16_t CONNECTION_TIMEOUT_MS = 2000;

  static constexpr uint8_t DEFAULT_LAST_COMM_TIMESTAMP = 0;
  uint32_t lastCommTimestamp = DEFAULT_LAST_COMM_TIMESTAMP;

  float primaryThermistorTempF = 0.0;
  float secondaryThermistorTempF = 0.0;

  SerialTransfer* receiver;

  JLed* receiveLed;
  JLed* connectionLed;

  public:

  void begin();

  void run();

  bool isConnected();

  float primaryThermistorTemperatureFahrenheit();
  
  float secondaryThermistorTemperatureFahrenheit();
};

#endif // !CALIBRATION_BOARD_RECEIVER_H