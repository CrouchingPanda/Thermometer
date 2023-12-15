#include "TouchSurface.h"

#include <Arduino.h>

#include <lvgl.h>
#include <Adafruit_FT6206.h>

#include "ScreenSpec.h"

TouchSurface* TouchSurface::getInstance() {
  static TouchSurface* instance = new TouchSurface();
  return instance;
}

void TouchSurface::begin() {
  Wire.setSDA(TOUCHSCREEN_SDA_PIN);
  Wire.setSCL(TOUCHSCREEN_SCL_PIN);
  touchSurface.begin();
}

void TouchSurface::registerWithLvgl() {
  static lv_indev_drv_t touchscreenDriver;
  lv_indev_drv_init(&touchscreenDriver);
  touchscreenDriver.type = LV_INDEV_TYPE_POINTER;
  touchscreenDriver.read_cb = readTouchpad;
  lv_indev_drv_register(&touchscreenDriver);
}

void TouchSurface::onTouch(OnTouchCallback callback) {
  if (callback == nullptr) return;
  onTouchCallback = callback;
}

void TouchSurface::readTouchpad(lv_indev_drv_t* _, lv_indev_data_t* touchData) {
  if (touchSurface.touched()) {
    touchData->state = LV_INDEV_STATE_PR;
    TS_Point touchpoint = touchSurface.getPoint();
    
    // mapping to work with correct screen orientation
    touchData->point.x = touchpoint.y;
    touchData->point.y = map(touchpoint.x, SCREEN_HEIGHT, 0, 0, SCREEN_HEIGHT);

    if (onTouchCallback != nullptr) onTouchCallback();
  } else {
    touchData->state = LV_INDEV_STATE_REL;
  }
}
