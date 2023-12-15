#include "Display.h"

#include <Arduino.h>

#include <LittleFS.h>

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <jled.h>

#include "ScreenSpec.h"

Display* Display::getInstance() {
  static Display* instance = new Display();
  return instance;
}

void Display::begin() {
  display.begin();
  display.setRotation(3); // flipped-over landscape
  initSpiDma();
  
  File file = LittleFS.open(DEFAULT_BRIGHTNESS_FILE, "r");
  if (file) {
    backlightDefaultBrightness = file.parseInt();
    file.close();
  }
  backlight.MaxBrightness(backlightDefaultBrightness);
  backlight.FadeOn(BACKLIGHT_BRIGHTNESS_CHANGE_MS);
}

void Display::initSpiDma() {
  bool dmaChannels[DMA_CHANNEL_COUNT];
  
  for (uint8_t i = 0; i < DMA_CHANNEL_COUNT; i++) {
    dmaChannels[i] = dma_channel_is_claimed(i);
  }
  
  display.initDMA();

  static uint8_t spiDmaChannel = 0;

  for (uint8_t i = 0; i < DMA_CHANNEL_COUNT; i++) {
    if (dma_channel_is_claimed(i) != dmaChannels[i]) {
      spiDmaChannel = i;
      break;
    }
  }

  dma_channel_set_irq0_enabled(spiDmaChannel, true);

  irq_set_exclusive_handler(
    DMA_IRQ_0,
    []() {
      lv_disp_flush_ready(&displayDriver);
      dma_channel_acknowledge_irq0(spiDmaChannel);
    }
  );

  irq_set_enabled(DMA_IRQ_0, true);
}

void Display::showLoadingScreen() {
  loadingScreen.begin(&display);
  while (backlight.Update());
}

void Display::setLoadingPercent(uint8_t percent) {
  loadingScreen.setLoadingPercent(percent);
}

void Display::switchFromLoadingScreen() {
  backlight.FadeOff(BACKLIGHT_BRIGHTNESS_CHANGE_MS);
  while (backlight.Update());
  backlight.FadeOn(BACKLIGHT_BRIGHTNESS_CHANGE_MS);
}

void Display::registerWithLvgl() {
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

void Display::initDefaultPowerMode() {
  if (powerMode == DEFAULT) return;
  backlight.FadeOn(BACKLIGHT_BRIGHTNESS_CHANGE_MS);
  powerMode = DEFAULT;
}

void Display::initLowPowerMode() {
  if (powerMode == LOW) return;
  backlight.MinBrightness(BACKLIGHT_LOW_BRIGHTNESS);
  backlight.FadeOff(BACKLIGHT_BRIGHTNESS_CHANGE_MS);
  powerMode = LOW;
}

void Display::flushBufferToDisplay(
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

void Display::run() {
  backlight.Update();
}

uint8_t Display::getDefaultBrightness() {
  return backlightDefaultBrightness;
}

void Display::setDefaultBrightness(uint8_t newBrightness) {
  if (backlightDefaultBrightness == newBrightness) return;
  if (BACKLIGHT_LOW_BRIGHTNESS > newBrightness) newBrightness = BACKLIGHT_LOW_BRIGHTNESS;

  uint8_t currentBrightness = backlightDefaultBrightness;
  backlightDefaultBrightness = newBrightness;
  backlight.MaxBrightness(newBrightness);
  backlight.Fade(currentBrightness, newBrightness, BACKLIGHT_BRIGHTNESS_CHANGE_MS);

  File file = LittleFS.open(DEFAULT_BRIGHTNESS_FILE, "w+");
  if (file) {
    file.print(backlightDefaultBrightness);
    file.close();
  }
}
