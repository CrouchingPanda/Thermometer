#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <jled.h>

#include "../config/ConfigStore.h"
#include "LoadingScreen.h"

class DisplayScreen {

  static TFT_eSPI display;

  ConfigStore& config;

  LoadingScreen loadingScreen;

  JLed backlight;
  
  enum PowerMode { DEFAULT, LOW } powerMode = DEFAULT;

  uint8_t backlightBrightness;

  public:

  DisplayScreen(ConfigStore& config);

  void begin();

  void showLoadingScreen();

  void setLoadingPercent(uint8_t percent);

  void switchFromLoadingScreen();

  void registerWithLvgl();

  void initDefaultPowerMode();

  void initLowPowerMode();

  uint8_t getDefaultBrightness();

  void setDefaultBrightness(uint8_t value);

  void run();

  private:

  void initSpiDma();

  static void flushBufferToDisplay(
    lv_disp_drv_t* displayDriver,
    const lv_area_t* displayArea,
    lv_color_t* pixels
  );
};

extern DisplayScreen Display;

#endif // !DISPLAY_H
