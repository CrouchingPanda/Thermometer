#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>

#include "ScreenSpec.h"
#include "gui/theme.h"

class LoadingScreen {

  static uint16_t colorHexTo16(uint32_t color) {
    return lv_color_to16(lv_color_hex(color));
  };

  static inline uint16_t SCREEN_BACKGROUND_COLOR = colorHexTo16(BACKGROUND_COLOR);
  static inline uint16_t BAR_WRAPPER_COLOR = colorHexTo16(ACTIVE_BACKGROUND_COLOR);
  static inline uint16_t BAR_COLOR = colorHexTo16(PRIMARY_COLOR);

  static constexpr uint16_t BAR_WRAPPER_THICKNESS = SCREEN_HEIGHT / 10;
  static constexpr uint16_t BAR_WRAPPER_LENGTH = SCREEN_WIDTH - PADDING * 2;
  static constexpr uint16_t BAR_WRAPPER_X = PADDING;
  static constexpr uint16_t BAR_WRAPPER_Y = (SCREEN_HEIGHT - BAR_WRAPPER_THICKNESS) / 2;
  static constexpr uint16_t BAR_WRAPPER_RADIUS = BAR_WRAPPER_THICKNESS / 2;

  static constexpr uint16_t BAR_THICKNESS = BAR_WRAPPER_THICKNESS - PADDING * 2;
  static constexpr uint16_t BAR_FULL_LENGTH = BAR_WRAPPER_LENGTH - PADDING * 2;
  static constexpr uint16_t BAR_X = BAR_WRAPPER_X + PADDING;
  static constexpr uint16_t BAR_Y = BAR_WRAPPER_Y + PADDING;
  static constexpr uint16_t BAR_RADIUS = BAR_THICKNESS / 2;

  TFT_eSPI* display;

  public:

  void begin(TFT_eSPI* display);

  void setLoadingPercent(uint8_t value);
};

#endif // !LOADING_SCREEN_H
