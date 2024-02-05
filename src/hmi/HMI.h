#ifndef HMI_H
#define HMI_H

#include <stdint.h>

#include <Arduino.h>

#include <lvgl.h>

#include "Display.h"
#include "TouchSurface.h"
#include "gui/pub_sub.h"

// this class acts as a bridge between C LVGL code and C++ Arduino
class PublicHMI {

  DisplayScreen& display;

  TouchSurface& touchSurface;

  public:

  PublicHMI(DisplayScreen& display, TouchSurface& touchSurface) :
    display(display),
    touchSurface(touchSurface) {};

  using TemperatureWriter = float(*)();
  void setObjectTemperatureWriter(TemperatureWriter callback);
  void setRoomTemperatureWriter(TemperatureWriter callback);
  void setOnDieTemperatureWriter(TemperatureWriter callback);
  void setBatteryTemperatureWriter(TemperatureWriter callback);
  void setPrimaryThermistorTemperatureWriter(TemperatureWriter callback);
  void setSecondaryThermistorTemperatureWriter(TemperatureWriter callback);

  using EmissivityWriter = float(*)();
  void setEmissivityWriter(EmissivityWriter callback);

  using EmissivityReader = void(*)(float emissivity);
  void setEmissivityReader(EmissivityReader callback);

  using BatteryPercentageWriter = uint8_t(*)();
  void setBatteryPercentageWriter(BatteryPercentageWriter callback);

  using BatteryChargingStatusWriter = bool(*)();
  void setBatteryChargingStatusWriter(BatteryChargingStatusWriter callback);

  using ConnectionStatusWriter = bool(*)();
  void setCalibrationBoardConnectionStatusWriter(ConnectionStatusWriter callback);

  using OnActivityCallback = void(*)();
  void onActivity(OnActivityCallback callback);

  using TimeoutWriter = uint8_t(*)();
  void setLowPowerModeTimeoutWriter(TimeoutWriter callback);
  void setPowerOffTimeoutWriter(TimeoutWriter callback);

  using TimeoutReader = void(*)(uint8_t minutes);
  void setLowPowerModeTimeoutReader(TimeoutReader callback);
  void setPowerOffTimeoutReader(TimeoutReader callback);

  using BrightnessWriter = uint8_t(*)();
  void setPowerIndicatorOnBrightnessWriter(BrightnessWriter callback);
  void setPowerIndicatorMaxBreathBrightnessWriter(BrightnessWriter callback);

  using BrightnessReader = void(*)(uint8_t brightness);
  void setPowerIndicatorOnBrightnessReader(BrightnessReader callback);
  void setPowerIndicatorMaxBreathBrightnessReader(BrightnessReader callback);

  using SmoothingFactorWriter = uint8_t(*)();
  void setObjectTemperatureSmoothingFactorWriter(SmoothingFactorWriter callback);

  using SmoothingFactorReader = void(*)(uint8_t factor);
  void setObjectTemperatureSmoothingFactorReader(SmoothingFactorReader callback);

  using TemperatureOffsetWriter = float(*)();
  void setRoomTemperatureOffsetWriter(TemperatureOffsetWriter callback);

  using TemperatureOffsetReader = void(*)(float offset);
  void setRoomTemperatureOffsetReader(TemperatureOffsetReader callback);

  void initLowPowerMode();

  void initDefaultMode();

  void begin();

  void showLoadingScreen();

  void setLoadingPercent(uint8_t percent);

  void switchFromLoadingScreen();

  void startLvgl();

  void run();

  private:

  struct TemperatureWriterTimerDTO {
    Message message;
    TemperatureWriter getTemperature;
    uint16_t frequencyMs;
  };

  void makeTemperatureWriterTimer(TemperatureWriterTimerDTO* dto);

  void setTimeoutWriter(Message message, TimeoutWriter callback);

  void makeTimeoutReaderSubscription(Message message, TimeoutReader callback);

  void setBrightnessWriter(Message message, BrightnessWriter callback);

  void makeBrightnessReaderSubscription(Message message, BrightnessReader callback);

  void registerDisplayHooksWithLvgl();
};

extern PublicHMI HMI;

#endif  // !HMI_H
