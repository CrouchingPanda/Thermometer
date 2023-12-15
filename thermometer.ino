#include <Wire.h>
#include <LittleFS.h>

#include "src/hmi/HMI.h"
#include "src/calibration/CalibrationBoardReceiver.h"
#include "src/hardware/MelexisThermometer.h"
#include "src/hardware/Battery.h"
#include "src/hardware/PowerButtonLED.h"
#include "src/monitor/HealthMonitor.h"
#include "src/monitor/ActivityMonitor.h"

// faster doesn't work, not exactly sure why
constexpr uint16_t I2C_FREQUENCY = 25'000;

static auto hmi = HMI();
static auto calibrationReceiver = CalibrationBoardReceiver();
static auto thermometer = MelexisThermometer();
static auto battery = Battery();
static auto powerButtonLED = PowerButtonLED();
static auto healthMonitor = HealthMonitor();
static auto activityMonitor = ActivityMonitor();

void setup() {
  LittleFS.begin();

  hmi.begin();  
  hmi.showLoadingScreen();
  hmi.setLoadingPercent(10);

  healthMonitor.begin();
  hmi.setLoadingPercent(20);

  powerButtonLED.begin();
  hmi.setLoadingPercent(30);

  activityMonitor.begin();
  hmi.setLoadingPercent(40);

  Wire1.setClock(I2C_FREQUENCY);
  hmi.setLoadingPercent(50);

  thermometer.begin(&Wire1);
  hmi.setLoadingPercent(60);

  battery.begin(&Wire1);
  hmi.setLoadingPercent(70);

  calibrationReceiver.begin();
  hmi.setLoadingPercent(80);

  hmi.startLvgl();
  hmi.setLoadingPercent(90);

  hmi.setObjectTemperatureWriter(
    [](){ return thermometer.objectTemperatureFahrenheit(); }
  );

  hmi.setRoomTemperatureWriter(
    [](){ return thermometer.roomTemperatureFahrenheit(); }
  );

  hmi.setOnDieTemperatureWriter(
    [](){ return thermometer.onDieTemperatureFahrenheit(); }
  );

  hmi.setEmissivityWriter(
    [](){ return thermometer.getEmissivity(); }
  );

  hmi.setEmissivityReader(
    [](float emissivity){ thermometer.setEmissivity(emissivity); }
  );

  hmi.setBatteryPercentageWriter(
    [](){ return battery.chargePercentage(); }
  );

  hmi.setBatteryChargingStatusWriter(
    [](){ return battery.isCharging(); }
  );

  hmi.setBatteryTemperatureWriter(
    [](){ return battery.temperatureFahrenheit(); }
  );

  hmi.setCalibrationBoardConnectionStatusWriter(
    [](){ return calibrationReceiver.isConnected(); }
  );

  hmi.setPrimaryThermistorTemperatureWriter(
    [](){ return calibrationReceiver.primaryThermistorTemperatureFahrenheit(); }
  );

  hmi.setSecondaryThermistorTemperatureWriter(
    [](){ return calibrationReceiver.secondaryThermistorTemperatureFahrenheit(); }
  );

  hmi.onActivity(
    [](){ activityMonitor.registerActivity(); }
  );

  hmi.setLowPowerModeTimeoutWriter(
    [](){ return activityMonitor.getLowPowerTimeoutMins(); }
  );

  hmi.setPowerOffTimeoutWriter(
    [](){ return activityMonitor.getPowerOffTimeoutMins(); }
  );

  hmi.setLowPowerModeTimeoutReader(
    [](uint8_t minutes){ activityMonitor.setLowPowerTimeoutMins(minutes); }
  );

  hmi.setPowerOffTimeoutReader(
    [](uint8_t minutes){ activityMonitor.setPowerOffTimeoutMins(minutes); }
  );

  hmi.setPowerIndicatorOnBrightnessWriter(
    [](){ return powerButtonLED.getOnBrightness(); }
  );

  hmi.setPowerIndicatorMaxBreathBrightnessWriter(
    [](){ return powerButtonLED.getMaxBreathBrightness(); }
  );

  hmi.setPowerIndicatorOnBrightnessReader(
    [](uint8_t brightness){ powerButtonLED.setOnBrightness(brightness); }
  );

  hmi.setPowerIndicatorMaxBreathBrightnessReader(
    [](uint8_t brightness){ powerButtonLED.setMaxBreathBrightness(brightness); }
  );

  hmi.setObjectTemperatureSmoothingFactorWriter(
    []() { return thermometer.getObjectTemperatureSmoothingFactor(); }
  );

  hmi.setObjectTemperatureSmoothingFactorReader(
    [](uint8_t factor) { thermometer.setObjectTemperatureSmoothingFactor(factor); }
  );

  hmi.setRoomTemperatureOffsetWriter(
    []() { return thermometer.getRoomTemperatureOffsetCelsius(); }
  );

  hmi.setRoomTemperatureOffsetReader(
    [](float offset) { thermometer.setRoomTemperatureOffsetCelsius(offset); }
  );

  activityMonitor.onLowPowerMode(
    [](){ hmi.initLowPowerMode(); }
  );

  activityMonitor.onDefaultMode(
    [](){ hmi.initDefaultMode(); }
  );

  hmi.setLoadingPercent(100);
  hmi.switchFromLoadingScreen();
}

void loop() {
  battery.isCharging() ? powerButtonLED.breathe() : powerButtonLED.on();
  healthMonitor.run();
  calibrationReceiver.run();
  activityMonitor.run();
  hmi.run();
}