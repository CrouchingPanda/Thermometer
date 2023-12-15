#include "CalibrationBoardReceiver.h"

#include <Arduino.h>

#include <jLed.h>
#include <SerialTransfer.h>
#include <InterboardSerial.h>

void CalibrationBoardReceiver::begin() {
  Serial2.setRX(SERIAL_RX_PIN);
  Serial2.setTX(SERIAL_TX_PIN);
  Serial2.begin(SERIAL_BAUD_RATE);
  
  receiver = new SerialTransfer();
  receiver->begin(Serial2);

  receiveLed = new JLed(RECEIVE_LED_PIN);
  connectionLed = new JLed(CONNECTION_LED_PIN);
  connectionLed->MaxBrightness(CONNECTION_LED_BRIGHTNESS);
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
  connectionLed->Update();
  receiveLed->Update();

  isConnected() ? connectionLed->On() : connectionLed->Off();

  if (!receiver->available()) return;
  
  DTO dto;
  receiver->rxObj(dto);
  primaryThermistorTempF = dto.primaryThermistorTemperatureFahrenheit;
  secondaryThermistorTempF = dto.secondaryThermistorTemperatureFahrenheit;
  
  lastCommTimestamp = millis();
  
  receiveLed->Blink(RECEIVE_LED_BLINK_ON_MS, 1);
}

