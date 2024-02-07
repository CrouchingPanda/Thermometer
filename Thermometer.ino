#include <Wire.h>

#include "src/hmi/HMI.h"
#include "src/calibration/CalibrationBoardReceiver.h"
#include "src/hardware/MelexisThermometer.h"
#include "src/hardware/Battery.h"
#include "src/hardware/PowerButtonLED.h"
#include "src/monitor/HealthMonitor.h"
#include "src/monitor/ActivityMonitor.h"

// faster doesn't work, not exactly sure why
constexpr uint32_t I2C_FREQUENCY = 70'000;

void setup() {
  HMI.begin();
  HMI.showLoadingScreen();
  HMI.setLoadingPercent(10);

  HealthMonitor.begin();
  HMI.setLoadingPercent(20);

  PowerLED.begin();
  HMI.setLoadingPercent(30);

  ActivityMonitor.begin();
  HMI.setLoadingPercent(40);

  Wire1.setClock(I2C_FREQUENCY);
  HMI.setLoadingPercent(50);

  Thermometer.begin();
  HMI.setLoadingPercent(60);

  LiPoBattery.begin();
  HMI.setLoadingPercent(70);

  CalibrationReceiver.begin();
  HMI.setLoadingPercent(80);

  HMI.startLvgl();
  HMI.setLoadingPercent(90);

  HMI.setObjectTemperatureWriter(
    [](){ return Thermometer.objectTemperatureFahrenheit(); }
  );

  HMI.setRoomTemperatureWriter(
    [](){ return Thermometer.roomTemperatureFahrenheit(); }
  );

  HMI.setOnDieTemperatureWriter(
    [](){ return Thermometer.onDieTemperatureFahrenheit(); }
  );

  HMI.setEmissivityWriter(
    [](){ return Thermometer.getEmissivity(); }
  );

  HMI.setEmissivityReader(
    [](float emissivity){ Thermometer.setEmissivity(emissivity); }
  );

  HMI.setBatteryPercentageWriter(
    [](){ return LiPoBattery.chargePercentage(); }
  );

  HMI.setBatteryChargingStatusWriter(
    [](){ return LiPoBattery.isCharging(); }
  );

  HMI.setBatteryTemperatureWriter(
    [](){ return LiPoBattery.temperatureFahrenheit(); }
  );

  HMI.setCalibrationBoardConnectionStatusWriter(
    [](){ return CalibrationReceiver.isConnected(); }
  );

  HMI.setPrimaryThermistorTemperatureWriter(
    [](){ return CalibrationReceiver.primaryThermistorTemperatureFahrenheit(); }
  );

  HMI.setSecondaryThermistorTemperatureWriter(
    [](){ return CalibrationReceiver.secondaryThermistorTemperatureFahrenheit(); }
  );

  HMI.onActivity(
    [](){ ActivityMonitor.registerActivity(); }
  );

  HMI.setLowPowerModeTimeoutWriter(
    [](){ return ActivityMonitor.getLowPowerTimeoutMins(); }
  );

  HMI.setPowerOffTimeoutWriter(
    [](){ return ActivityMonitor.getPowerOffTimeoutMins(); }
  );

  HMI.setLowPowerModeTimeoutReader(
    [](uint8_t minutes){ ActivityMonitor.setLowPowerTimeoutMins(minutes); }
  );

  HMI.setPowerOffTimeoutReader(
    [](uint8_t minutes){ ActivityMonitor.setPowerOffTimeoutMins(minutes); }
  );

  HMI.setPowerIndicatorOnBrightnessWriter(
    [](){ return PowerLED.getOnBrightness(); }
  );

  HMI.setPowerIndicatorMaxBreathBrightnessWriter(
    [](){ return PowerLED.getMaxBreathBrightness(); }
  );

  HMI.setPowerIndicatorOnBrightnessReader(
    [](uint8_t brightness){ PowerLED.setOnBrightness(brightness); }
  );

  HMI.setPowerIndicatorMaxBreathBrightnessReader(
    [](uint8_t brightness){ PowerLED.setMaxBreathBrightness(brightness); }
  );

  HMI.setObjectTemperatureSmoothingFactorWriter(
    []() { return Thermometer.getObjectTemperatureSmoothingFactor(); }
  );

  HMI.setObjectTemperatureSmoothingFactorReader(
    [](uint8_t factor) { Thermometer.setObjectTemperatureSmoothingFactor(factor); }
  );

  HMI.setRoomTemperatureOffsetWriter(
    []() { return Thermometer.getRoomTemperatureOffsetCelsius(); }
  );

  HMI.setRoomTemperatureOffsetReader(
    [](float offset) { Thermometer.setRoomTemperatureOffsetCelsius(offset); }
  );

  ActivityMonitor.onLowPowerMode(
    [](){ HMI.initLowPowerMode(); }
  );

  ActivityMonitor.onDefaultMode(
    [](){ HMI.initDefaultMode(); }
  );

  HMI.setLoadingPercent(100);
  HMI.switchFromLoadingScreen();
}

void loop() {
  LiPoBattery.isCharging() ? PowerLED.breathe() : PowerLED.on();
  HealthMonitor.run();
  CalibrationReceiver.run();
  ActivityMonitor.run();
  HMI.run();
}