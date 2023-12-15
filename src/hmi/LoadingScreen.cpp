#include "LoadingScreen.h"

#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>

#include "ScreenSpec.h"

void LoadingScreen::begin(TFT_eSPI* display) {
  this->display = display;
  display->fillScreen(SCREEN_BACKGROUND_COLOR);
  display->fillSmoothRoundRect(
    BAR_WRAPPER_X,
    BAR_WRAPPER_Y,
    BAR_WRAPPER_LENGTH,
    BAR_WRAPPER_THICKNESS,
    BAR_WRAPPER_RADIUS,
    BAR_WRAPPER_COLOR,
    SCREEN_BACKGROUND_COLOR
  );
}

void LoadingScreen::setLoadingPercent(uint8_t percent) {
  if (percent > 100) percent = 100;
  display->fillSmoothRoundRect(
    BAR_X,
    BAR_Y,
    BAR_FULL_LENGTH * percent / 100,
    BAR_THICKNESS,
    BAR_RADIUS,
    BAR_COLOR,
    BAR_WRAPPER_COLOR
  );
}