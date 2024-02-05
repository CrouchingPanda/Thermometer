#include "CalibrationBoardReceiver.h"

#include <stdint.h>

#include <Arduino.h>

#include <jLed.h>
#include <SerialTransfer.h>
#include <InterboardSerial.h>

constexpr uint8_t CONNECTION_LED_PIN = 11;
constexpr uint8_t CONNECTION_LED_BRIGHTNESS = 50;

constexpr uint8_t RECEIVE_LED_PIN = 10;
constexpr uint8_t RECEIVE_LED_BLINK_ON_MS = 10;

constexpr uint8_t SERIAL_RX_PIN = 9;
constexpr uint8_t SERIAL_TX_PIN = 8;
constexpr uint16_t CONNECTION_TIMEOUT_MS = 2000;

constexpr uint8_t DEFAULT_LAST_COMM_TIMESTAMP = 0;

CalibrationBoardReceiver::CalibrationBoardReceiver() :
  lastCommTimestamp(DEFAULT_LAST_COMM_TIMESTAMP),
  receiver(SerialTransfer()),
  receiveLed(JLed(RECEIVE_LED_PIN)),
  connectionLed(JLed(CONNECTION_LED_PIN)) {};

void CalibrationBoardReceiver::begin() {
  Serial2.setRX(SERIAL_RX_PIN);
  Serial2.setTX(SERIAL_TX_PIN);
  Serial2.begin(SERIAL_BAUD_RATE);
  
  receiver.begin(Serial2);

  connectionLed.MaxBrightness(CONNECTION_LED_BRIGHTNESS);
}

bool CalibrationBoardReceiver::isConnected() {
  if (lastCommTimestamp == DEFAULT_LAST_COMM_TIMESTAMP) return false;
  return millis() - lastCommTimestamp < CONNECTION_TIMEOUT_MS;
}

float CalibrationBoardReceiver::primaryThermistorTemperatureFahrenheit() {
  return primaryThermistorTempF;
}

float CalibrationBoardReceiver::secondaryThermistorTemperatureFahrenheit() {
  return secondaryThermistorTempF;
}

void CalibrationBoardReceiver::run() {
  connectionLed.Update();
  receiveLed.Update();

  isConnected() ? connectionLed.On() : connectionLed.Off();

  if (!receiver.available()) return;
  
  DTO dto;
  receiver.rxObj(dto);
  primaryThermistorTempF = dto.primaryThermistorTemperatureFahrenheit;
  secondaryThermistorTempF = dto.secondaryThermistorTemperatureFahrenheit;
  
  lastCommTimestamp = millis();
  
  receiveLed.Blink(RECEIVE_LED_BLINK_ON_MS, 1);
}

CalibrationBoardReceiver CalibrationReceiver = CalibrationBoardReceiver();

