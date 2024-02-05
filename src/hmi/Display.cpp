#include "Display.h"

#include <stdint.h>
#include <stdlib.h>

#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <jled.h>

#include "../config/ConfigStore.h"
#include "LoadingScreen.h"
#include "ScreenSpec.h"

constexpr uint8_t DMA_CHANNEL_COUNT = 12;

constexpr const char* BACKLIGHT_BRIGHTNESS_CONFIG_KEY = "displayDefaultBrightness";
constexpr uint8_t BACKLIGHT_PIN = A3;
constexpr uint8_t BACKLIGHT_LOW_BRIGHTNESS = 16;
constexpr uint8_t BACKLIGHT_DEFAULT_BRIGHTNESS = 130;
constexpr uint16_t BACKLIGHT_BRIGHTNESS_CHANGE_MS = 500;

// optimized for the amount of RAM
constexpr uint16_t DISPLAY_BUFFER_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT / 2 + 12'500;

static lv_color_t firstDrawBuffer[DISPLAY_BUFFER_SIZE];
static lv_color_t secondDrawBuffer[DISPLAY_BUFFER_SIZE];

static lv_disp_drv_t displayDriver;
static lv_disp_draw_buf_t displayDrawBuffer;

TFT_eSPI DisplayScreen::display = TFT_eSPI(SCREEN_HEIGHT, SCREEN_WIDTH);

DisplayScreen::DisplayScreen(ConfigStore& config) :
  config(config),
  backlight(JLed(BACKLIGHT_PIN)), 
  loadingScreen(LoadingScreen(display)) {};

void DisplayScreen::begin() {
  display.begin();
  display.setRotation(3); // flipped-over landscape
  initSpiDma();
  
  backlightBrightness = config.getOrDefault(
    BACKLIGHT_BRIGHTNESS_CONFIG_KEY, 
    BACKLIGHT_DEFAULT_BRIGHTNESS
  );
  
  backlight.MaxBrightness(backlightBrightness);
  backlight.FadeOn(BACKLIGHT_BRIGHTNESS_CHANGE_MS);
}

static void setDmaCompletionInterrupt(uint8_t dmaChannel) {
  dma_channel_set_irq0_enabled(dmaChannel, true);

  // static to be able to get captured in the lambda below
  static uint8_t channel = dmaChannel;

  // LVGL needs be told to start the next pixel buffer flush
  // once the last DMA pixel transfer is done
  irq_set_exclusive_handler(
    DMA_IRQ_0,
    []() {
      lv_disp_flush_ready(&displayDriver);
      dma_channel_acknowledge_irq0(channel);
    }
  );

  irq_set_enabled(DMA_IRQ_0, true);
}

void DisplayScreen::initSpiDma() {
  bool dmaChannels[DMA_CHANNEL_COUNT]; // "true" = captured
  
  for (size_t i = 0; i < DMA_CHANNEL_COUNT; ++i) {
    dmaChannels[i] = dma_channel_is_claimed(i);
  }
  
  // one DMA channel gets claimed here; we need to find which one
  display.initDMA();

  uint8_t claimedDmaChannel = 0;

  for (size_t i = 0; i < DMA_CHANNEL_COUNT; ++i) {
    if (dma_channel_is_claimed(i) != dmaChannels[i]) {
      claimedDmaChannel = i;
      break;
    }
  }

  setDmaCompletionInterrupt(claimedDmaChannel);
}

void DisplayScreen::showLoadingScreen() {
  loadingScreen.begin();
  while (backlight.Update());
}

void DisplayScreen::setLoadingPercent(uint8_t percent) {
  loadingScreen.setLoadingPercent(percent);
}

void DisplayScreen::switchFromLoadingScreen() {
  backlight.FadeOff(BACKLIGHT_BRIGHTNESS_CHANGE_MS);
  while (backlight.Update());
  backlight.FadeOn(BACKLIGHT_BRIGHTNESS_CHANGE_MS);
}

void DisplayScreen::registerWithLvgl() {
  lv_disp_draw_buf_init(
    &displayDrawBuffer,
    firstDrawBuffer,
    secondDrawBuffer,
    DISPLAY_BUFFER_SIZE
  );

  lv_disp_drv_init(&displayDriver);
  displayDriver.hor_res = SCREEN_WIDTH;
  displayDriver.ver_res = SCREEN_HEIGHT;
  displayDriver.flush_cb = flushBufferToDisplay;
  displayDriver.draw_buf = &displayDrawBuffer;
  lv_disp_drv_register(&displayDriver);
}

void DisplayScreen::initDefaultPowerMode() {
  if (powerMode == DEFAULT) return;
  backlight.FadeOn(BACKLIGHT_BRIGHTNESS_CHANGE_MS);
  powerMode = DEFAULT;
}

void DisplayScreen::initLowPowerMode() {
  if (powerMode == LOW) return;
  backlight.MinBrightness(BACKLIGHT_LOW_BRIGHTNESS);
  backlight.FadeOff(BACKLIGHT_BRIGHTNESS_CHANGE_MS);
  powerMode = LOW;
}

void DisplayScreen::flushBufferToDisplay(
  lv_disp_drv_t* _,
  const lv_area_t* displayArea,
  lv_color_t* pixels
) {
  uint16_t width = displayArea->x2 - displayArea->x1 + 1;
  uint16_t height = displayArea->y2 - displayArea->y1 + 1;
  display.startWrite();
  display.setAddrWindow(displayArea->x1, displayArea->y1, width, height);
  display.setSwapBytes(true);
  display.pushPixelsDMA((uint16_t*) &pixels->full, width * height);
}

void DisplayScreen::run() {
  backlight.Update();
}

uint8_t DisplayScreen::getDefaultBrightness() {
  return backlightBrightness;
}

void DisplayScreen::setDefaultBrightness(uint8_t newBrightness) {
  if (backlightBrightness == newBrightness) return;
  if (BACKLIGHT_LOW_BRIGHTNESS > newBrightness) newBrightness = BACKLIGHT_LOW_BRIGHTNESS;

  uint8_t currentBrightness = backlightBrightness;
  backlightBrightness = newBrightness;
  backlight.MaxBrightness(newBrightness);
  backlight.Fade(currentBrightness, newBrightness, BACKLIGHT_BRIGHTNESS_CHANGE_MS);

  config.set(BACKLIGHT_BRIGHTNESS_CONFIG_KEY, backlightBrightness);
}

DisplayScreen Display = DisplayScreen(Config);
