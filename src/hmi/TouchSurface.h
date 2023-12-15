#ifndef TOUCH_SURFACE_H
#define TOUCH_SURFACE_H

#include <Arduino.h>

#include <lvgl.h>
#include <Adafruit_FT6206.h>

class TouchSurface {

  static constexpr uint8_t TOUCHSCREEN_SDA_PIN = 0;
  static constexpr uint8_t TOUCHSCREEN_SCL_PIN = 1;

  static inline Adafruit_FT6206 touchSurface = Adafruit_FT6206();

  using OnTouchCallback = void(*)();
  static inline OnTouchCallback onTouchCallback = nullptr;

  public:

  static TouchSurface* getInstance();

  void begin();

  void registerWithLvgl();

  void onTouch(OnTouchCallback callback);

  static void readTouchpad(lv_indev_drv_t* touchpadDriver, lv_indev_data_t* touchData);
};

#endif // !TOUCH_SURFACE_H
