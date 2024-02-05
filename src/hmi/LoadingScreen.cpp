#include "LoadingScreen.h"

#include <stdint.h>

#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>

#include "ScreenSpec.h"
#include "gui/theme.h"

static uint16_t colorHexTo16(uint32_t color) {
  return lv_color_to16(lv_color_hex(color));
};

static uint16_t SCREEN_BACKGROUND_COLOR = colorHexTo16(BACKGROUND_COLOR);
static uint16_t ARC_WRAPPER_COLOR = colorHexTo16(ACTIVE_BACKGROUND_COLOR);
static uint16_t ARC_COLOR = colorHexTo16(PRIMARY_COLOR);

constexpr uint8_t ARC_WRAPPER_THICKNESS = SCREEN_HEIGHT / 12;
constexpr uint8_t ARC_WRAPPER_RADIUS = SCREEN_HEIGHT / 3;
constexpr uint8_t ARC_WRAPPER_START_ANGLE = 0;
constexpr uint16_t ARC_WRAPPER_END_ANGLE = 360;

constexpr uint8_t ARC_CENTER_X = SCREEN_WIDTH / 2;
constexpr uint8_t ARC_CENTER_Y = SCREEN_HEIGHT / 2;
constexpr uint8_t ARC_START_ANGLE = 0;
constexpr uint8_t ARC_RADIUS = ARC_WRAPPER_RADIUS + PADDING;
constexpr uint8_t ARC_THICKNESS = ARC_WRAPPER_THICKNESS - PADDING * 2;
constexpr uint8_t BAR_RADIUS = ARC_THICKNESS / 2;

void LoadingScreen::begin() {
  display.begin();
  display.setRotation(3); // flipped-over landscape

  display.fillScreen(SCREEN_BACKGROUND_COLOR);
  display.drawSmoothArc(
    ARC_CENTER_X,
    ARC_CENTER_Y,
    ARC_WRAPPER_RADIUS + ARC_WRAPPER_THICKNESS,
    ARC_WRAPPER_RADIUS,
    ARC_WRAPPER_START_ANGLE,
    ARC_WRAPPER_END_ANGLE,
    ARC_WRAPPER_COLOR,
    SCREEN_BACKGROUND_COLOR,
    false // no rounded ends
  );
}

void LoadingScreen::setLoadingPercent(uint8_t percent) {
  if (percent > 100) percent = 100;
  display.drawSmoothArc(
    ARC_CENTER_X,
    ARC_CENTER_Y,
    ARC_RADIUS + ARC_THICKNESS,
    ARC_RADIUS,
    ARC_START_ANGLE,
    360 * percent / 100,
    ARC_COLOR,
    ARC_WRAPPER_COLOR,
    true // round ends
  );
}