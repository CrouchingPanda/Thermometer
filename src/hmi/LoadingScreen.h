#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>

class LoadingScreen {

  TFT_eSPI& display;

  public:

  LoadingScreen(TFT_eSPI& display) : display(display) {};

  void begin();

  void setLoadingPercent(uint8_t value);
};

#endif // !LOADING_SCREEN_H
