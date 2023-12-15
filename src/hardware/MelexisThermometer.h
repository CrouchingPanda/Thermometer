#ifndef MELEXIS_THERMOMETER
#define MELEXIS_THERMOMETER

#include <Arduino.h>

#include <Wire.h>

#include <Smoothed.h>
#include <Adafruit_MLX90614.h>

class MelexisThermometer {

  static constexpr const char* EMISSIVITY_FILE = "melexis/emissivity";
  static constexpr const char* AMBIENT_CORRECTION_OFFSET_FILE = "melexis/ambient_correction";
  static constexpr const char* SMOOTHING_FACTOR_FILE = "melexis/smoothing_factor";

  static constexpr float MAX_EMISSIVITY = 1.0;
  static constexpr float MIN_EMISSIVITY = 0.1;

  static constexpr uint8_t MAX_SMOOTHING_FACTOR = 100;

  Smoothed<float>* smoother;
  
  uint8_t smoothingFactor = 10; // 1..MAX_SMOOTHING_FACTOR; more = less smooth

  Adafruit_MLX90614* sensor;

  float emissivity = MAX_EMISSIVITY;
  
  // assume the room temperature differs from die temperature by this number
  // see "Temperature reading dependence on VDD" chapter in datasheet
  float roomTemperatureOffsetCelsius = (3.3 - 3.0) * 0.6;

  public:

  void begin(TwoWire* wire);

  float objectTemperatureFahrenheit();

  float onDieTemperatureFahrenheit();
  
  float roomTemperatureFahrenheit();

  float getEmissivity();

  void setEmissivity(float value);

  float getRoomTemperatureOffsetCelsius();
  
  void setRoomTemperatureOffsetCelsius(float offset);

  uint8_t getObjectTemperatureSmoothingFactor();

  void setObjectTemperatureSmoothingFactor(uint8_t value);
};

#endif  // !MELEXIS_THERMOMETER