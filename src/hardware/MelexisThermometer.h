#ifndef MELEXIS_THERMOMETER
#define MELEXIS_THERMOMETER

#include <Arduino.h>

#include <Wire.h>

#include <Adafruit_MLX90614.h>

class MelexisThermometer {
  static constexpr uint8_t SENSOR_POWER_PIN = A3;

  static constexpr uint16_t SENSOR_RESET_POWER_DOWN_TIME_MS = 30;
  static constexpr uint16_t SENSOR_RESET_POWER_UP_TIME_MS = 300;

  static constexpr float MAX_EMISSIVITY = 1.0;
  static constexpr float MIN_EMISSIVITY = 0.1;

  Adafruit_MLX90614* sensor;

  float cachedEmissivity = MAX_EMISSIVITY;
  float cachedObjectTemperatureFarengheit = 73.0;
  float cachedAmbientTemperatureFarengheit = 73.0;

  bool sensorResetting = false;
  uint32_t sensorPoweredDownTimestamp = 0;
  uint32_t sensorPoweredUpTimestamp = 0;

  bool emissivitySettingPending = false;
  float pendingEmissivity = MAX_EMISSIVITY;

 public:

  void begin(TwoWire* wire);

  float objectTemperatureFarengheit();

  float ambientTemperatureFarengheit();

  float getEmissivity();

  void setEmissivity(float value);

 private:

  void handlePendingChanges();

  void setPendingEmissivity();

  void writeEmissivityToSensor(float value);

  // the sensor needs to be power cycled every time emissivity gets changed
  void powerDown();
  void tryPowerUp();
};

#endif  // !MELEXIS_THERMOMETER