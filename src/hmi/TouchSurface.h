#ifndef TOUCH_SURFACE_H
#define TOUCH_SURFACE_H

#include <Arduino.h>

#include <lvgl.h>
#include <Adafruit_FT6206.h>

class TouchSurface {

  static Adafruit_FT6206 touchSurface;

  using OnTouchCallback = void(*)();
  static OnTouchCallback onTouchCallback;

  public:

  void begin();

  void registerWithLvgl();

  void onTouch(OnTouchCallback callback);

  private:

  static void readTouchpad(lv_indev_drv_t* touchpadDriver, lv_indev_data_t* touchData);
};

extern TouchSurface CapTouchSurface;

#endif // !TOUCH_SURFACE_H
