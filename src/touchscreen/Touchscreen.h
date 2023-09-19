#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <Arduino.h>

#include <EEPROM.h>

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <Adafruit_FT6206.h>

#include "ReadingSmoother.h"
#include "gui/pub_sub.h"

// this class acts as a bridge between C-style LVGL code and C++ Arduino
class Touchscreen {

  static constexpr uint16_t SCREEN_WIDTH = 320;
  static constexpr uint8_t SCREEN_HEIGHT = 240;
  static constexpr uint16_t DISPLAY_BUFFER_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT / 2;

  // must be Wire pins (not Wire1 pins) - see pinout
  static constexpr uint8_t TOUCHSCREEN_SDA_PIN = 0;
  static constexpr uint8_t TOUCHSCREEN_SCL_PIN = 1;
  
  static constexpr uint8_t TEMPERATURE_UPDATE_FREQUENCY_MS = 100;
  static constexpr uint16_t BATTERY_CHARGE_PERCENTAGE_UPDATE_FREQUENCY_MS = 5000;
  static constexpr uint16_t BATTERY_CHARGING_STATUS_UPDATE_FREQUENCY_MS = 1000;

  static constexpr uint8_t OBJECT_TEMPERATURE_SMOOTHER_EEPROM_ADDRESS = 0;
  static constexpr uint8_t AMBIENT_TEMPERATURE_SMOOTHER_EEPROM_ADDRESS = 1;
  static constexpr uint8_t THERMISTOR_TEMPERATURE_SMOOTHER_EEPROM_ADDRESS = 2;

  static constexpr uint8_t DMA_CHANNEL_COUNT = 12;

  static inline TFT_eSPI display = TFT_eSPI(SCREEN_WIDTH, SCREEN_HEIGHT);
  static inline Adafruit_FT6206 touchSurface = Adafruit_FT6206();

  static inline lv_color_t firstDrawBuffer[DISPLAY_BUFFER_SIZE];
  static inline lv_color_t secondDrawBuffer[DISPLAY_BUFFER_SIZE];

  static inline lv_disp_drv_t displayDriver;
  static inline lv_disp_draw_buf_t displayDrawBuffer;

  static inline uint8_t spiDmaChannel = 0;

  static inline ReadingSmoother* objectTemperatureSmoother;
  static inline ReadingSmoother* ambientTemperatureSmoother;
  static inline ReadingSmoother* thermistorTemperatureSmoother;

  Touchscreen() {};

  public:

  static Touchscreen getInstance();

  using TemperatureWriterCallback = float(*)();
  void setObjectTemperatureWriter(TemperatureWriterCallback callback);
  void setAmbientTemperatureWriter(TemperatureWriterCallback callback);
  void setThermistorTemperatureWriter(TemperatureWriterCallback callback);

  using EmissivityWriterCallback = float(*)();
  void setEmissivityWriter(EmissivityWriterCallback callback);

  using EmissivityReaderCallback = void(*)(float emissivity);
  void setEmissivityReader(EmissivityReaderCallback callback);

  using BatteryPercentageWriter = uint8_t(*)();
  void setBatteryPercentageWriter(BatteryPercentageWriter callback);

  using BatteryChargingStatusWriter = bool(*)();
  void setBatteryChargingStatusWriter(BatteryChargingStatusWriter callback);

  void begin(EEPROMClass* EEPROM);

  void run();

  private:

  void initSmoothers(EEPROMClass* EEPROM);

  void initDisplay();

  void initTouchscreen();

  void initLVGLInfra();

  void setDisplayDmaHandler(bool* channels);

  static void flushBufferToDisplay(
    lv_disp_drv_t* displayDriver,
    const lv_area_t* displayArea,
    lv_color_t* pixels
  );

  static void readTouchpad(lv_indev_drv_t* touchpadDriver, lv_indev_data_t* touchData);
};

#endif  // !TOUCHSCREEN_H
