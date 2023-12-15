#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <jled.h>

#include "ScreenSpec.h"
#include "LoadingScreen.h"

class Display {

  private:

  static constexpr uint16_t DISPLAY_BUFFER_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT / 2;

  static constexpr uint8_t DMA_CHANNEL_COUNT = 12;

  static constexpr uint8_t BACKLIGHT_PIN = A3;
  static constexpr uint8_t BACKLIGHT_LOW_BRIGHTNESS = 16;
  static constexpr uint16_t BACKLIGHT_BRIGHTNESS_CHANGE_MS = 500;

  static constexpr const char* DEFAULT_BRIGHTNESS_FILE = "display/default_brightness";

  static inline lv_color_t firstDrawBuffer[DISPLAY_BUFFER_SIZE];
  static inline lv_color_t secondDrawBuffer[DISPLAY_BUFFER_SIZE];

  static inline lv_disp_drv_t displayDriver;
  static inline lv_disp_draw_buf_t displayDrawBuffer;

  static inline TFT_eSPI display = TFT_eSPI(SCREEN_HEIGHT, SCREEN_WIDTH);

  static inline LoadingScreen loadingScreen = LoadingScreen();

  static inline JLed backlight = JLed(BACKLIGHT_PIN);
  
  static inline enum PowerMode { DEFAULT, LOW } powerMode = DEFAULT;

  static inline uint8_t backlightDefaultBrightness = 127;

  Display() {};

  public:

  static Display* getInstance();

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

#endif // !DISPLAY_H
