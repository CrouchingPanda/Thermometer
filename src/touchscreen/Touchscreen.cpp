#include "Touchscreen.h"

#include "hardware/dma.h" // from RP2040 SDK
#include <Arduino.h>

#include <Wire.h>
#include <EEPROM.h>
#include <SPI.h>

#include <lvgl.h>

#include "ReadingSmoother.h"
#include "gui/ui.h"
#include "gui/pub_sub.h"

Touchscreen Touchscreen::getInstance() {
  static Touchscreen* instance = new Touchscreen();
  return *instance;
}

static void logging_init() {
  lv_log_register_print_cb(
    [](const char* message) {
      Serial.printf(message);
      Serial.flush();
    }
  );
}

void Touchscreen::begin(EEPROMClass* EEPROM) {
  lv_init(); // must come before display/Touchscreen init
  logging_init();
  initDisplay();
  initTouchscreen();
  initLVGLInfra();
  ui_init(); // must come after all else has been started
  initSmoothers(EEPROM);
}

static ReadingSmoother* initSmoother(EEPROMClass* EEPROM, uint8_t EEPROM_address) {
  ReadingSmoother* smoother = new ReadingSmoother(EEPROM, EEPROM_address);
  smoother->begin();
  return smoother;
}

static void sendSmoothingFactorMessage(Message message, ReadingSmoother* smoother) {
  uint8_t smoothingFactor = smoother->getFactor();
  lv_msg_send(message, &smoothingFactor);
}

static void handleNewSmoothingFactorSet(Message message, ReadingSmoother* smoother) {
  lv_msg_subscribe(
    message,
    [](void* _, lv_msg_t* message) {
      auto smoother = (ReadingSmoother*) lv_msg_get_user_data(message);
      uint8_t* newSmoothingFactor = (uint8_t*) lv_msg_get_payload(message);
      smoother->setFactor(*newSmoothingFactor);
    },
    (void*)smoother
  );
}

void Touchscreen::initSmoothers(EEPROMClass* EEPROM) {
  objectTemperatureSmoother = initSmoother(EEPROM, OBJECT_TEMPERATURE_SMOOTHER_EEPROM_ADDRESS);
  sendSmoothingFactorMessage(NEW_OBJECT_TEMPERATURE_SMOOTHING_MSG, objectTemperatureSmoother);
  handleNewSmoothingFactorSet(SET_OBJECT_TEMPERATURE_SMOOTHER_FACTOR_MSG, objectTemperatureSmoother);

  ambientTemperatureSmoother = initSmoother(EEPROM, AMBIENT_TEMPERATURE_SMOOTHER_EEPROM_ADDRESS);
  sendSmoothingFactorMessage(NEW_AMBIENT_TEMPERATURE_SMOOTHING_MSG, ambientTemperatureSmoother);
  handleNewSmoothingFactorSet(SET_AMBIENT_TEMPERATURE_SMOOTHER_FACTOR_MSG, ambientTemperatureSmoother);

  thermistorTemperatureSmoother = initSmoother(EEPROM, THERMISTOR_TEMPERATURE_SMOOTHER_EEPROM_ADDRESS);
  sendSmoothingFactorMessage(NEW_THERMISTOR_TEMPERATURE_SMOOTHING_MSG, thermistorTemperatureSmoother);
  handleNewSmoothingFactorSet(SET_THERMISTOR_TEMPERATURE_SMOOTHER_FACTOR_MSG, thermistorTemperatureSmoother);
}

void Touchscreen::initDisplay() {
  display.begin();
  display.setRotation(3); // flipped-over landscape

  bool dmaChannels[DMA_CHANNEL_COUNT];
  for (uint8_t i = 0; i < DMA_CHANNEL_COUNT; i++) dmaChannels[i] = dma_channel_is_claimed(i);

  display.initDMA();
  setDisplayDmaHandler(dmaChannels);
}

// tells LVGL SPI transfer to display is done when DMA completion IRQ is triggered
void Touchscreen::setDisplayDmaHandler(bool* channels) {
  for (uint8_t i = 0; i < DMA_CHANNEL_COUNT; i++) {
    if (dma_channel_is_claimed(i) != channels[i]) spiDmaChannel = i;
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

void Touchscreen::initTouchscreen() {
  Wire.setSDA(TOUCHSCREEN_SDA_PIN);
  Wire.setSCL(TOUCHSCREEN_SCL_PIN);
  touchSurface.begin();
}

void Touchscreen::initLVGLInfra() {
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

  static lv_indev_drv_t TouchscreenDriver;
  lv_indev_drv_init(&TouchscreenDriver);
  TouchscreenDriver.type = LV_INDEV_TYPE_POINTER;
  TouchscreenDriver.read_cb = readTouchpad;
  lv_indev_drv_register(&TouchscreenDriver);
}

void Touchscreen::flushBufferToDisplay(
  lv_disp_drv_t* _,
  const lv_area_t* displayArea,
  lv_color_t* pixels
) {
  uint32_t width = displayArea->x2 - displayArea->x1 + 1;
  uint32_t height = displayArea->y2 - displayArea->y1 + 1;
  display.startWrite();
  display.setSwapBytes(true);
  display.setAddrWindow(displayArea->x1, displayArea->y1, width, height);
  display.pushPixelsDMA((uint16_t*) &pixels->full, width * height);
}

void Touchscreen::readTouchpad(lv_indev_drv_t* _, lv_indev_data_t* touchData) {
  if (touchSurface.touched()) {
    touchData->state = LV_INDEV_STATE_PR;
    TS_Point touchpoint = touchSurface.getPoint();
    // mapping to work with correct screen orientation
    touchData->point.x = touchpoint.y;
    touchData->point.y = map(touchpoint.x, SCREEN_HEIGHT, 0, 0, SCREEN_HEIGHT);
  } else {
    touchData->state = LV_INDEV_STATE_REL;
  }
}

void Touchscreen::run() {
  uint32_t timeToNextRun = lv_timer_handler();
  delay(timeToNextRun);
}

void Touchscreen::setBatteryPercentageWriter(BatteryPercentageWriter callback) {
  static lv_timer_t* timer = nullptr;
  if (timer != nullptr) lv_timer_del(timer);

  timer = lv_timer_create(
    [](lv_timer_t* timer) {
      auto getBatteryPercentage = (BatteryPercentageWriter) timer->user_data;
      uint8_t percentage = getBatteryPercentage();
      lv_msg_send(NEW_BATTERY_PERCENTAGE_MSG, &percentage);
    },
    BATTERY_CHARGE_PERCENTAGE_UPDATE_FREQUENCY_MS,
    (void*) callback
  );

  uint8_t percentage = callback();
  lv_msg_send(NEW_BATTERY_PERCENTAGE_MSG, &percentage);
}

void Touchscreen::setBatteryChargingStatusWriter(BatteryChargingStatusWriter callback) {
  static lv_timer_t* timer = nullptr;
  if (timer != nullptr) lv_timer_del(timer);

  timer = lv_timer_create(
    [](lv_timer_t* timer) {
      auto getBatteryChargingStatus = (BatteryPercentageWriter) timer->user_data;
      bool batteryIsCharging = getBatteryChargingStatus();
      lv_msg_send(NEW_BATTERY_CHARGING_STATUS_MSG, &batteryIsCharging);
    },
    BATTERY_CHARGING_STATUS_UPDATE_FREQUENCY_MS,
    (void*) callback
  );

  bool batteryIsCharging = callback(); 
  lv_msg_send(NEW_BATTERY_CHARGING_STATUS_MSG, &batteryIsCharging);
}

void Touchscreen::setEmissivityWriter(EmissivityWriterCallback callback) {
  float emissivity = callback();
  lv_msg_send(NEW_EMISSIVITY_READING_MSG, &emissivity);
}

void Touchscreen::setEmissivityReader(EmissivityReaderCallback callback) {
  static void* subscription = nullptr;
  if (subscription != nullptr) lv_msg_unsubscribe(subscription);

  subscription = lv_msg_subscribe(
    SET_EMISSIVITY_MSG,
    [](void* _, lv_msg_t* message) {
      auto setEmissivity = (EmissivityReaderCallback) lv_msg_get_user_data(message);
      float* newEmissivity = (float*) lv_msg_get_payload(message);
      setEmissivity(*newEmissivity);
    },
    (void*) callback
  );
}

void Touchscreen::setAmbientTemperatureWriter(TemperatureWriterCallback callback) {
  static lv_timer_t* timer = nullptr;
  if (timer != nullptr) lv_timer_del(timer);

  timer = lv_timer_create(
    [](lv_timer_t* timer) {
      auto getTemperature = (TemperatureWriterCallback) timer->user_data;
      float temperature = getTemperature();
      ambientTemperatureSmoother->addReading(temperature);
      float smoothedTemperature = ambientTemperatureSmoother->smoothedValue();
      lv_msg_send(NEW_AMBIENT_TEMPERATURE_MSG, &smoothedTemperature);
    },
    TEMPERATURE_UPDATE_FREQUENCY_MS,
    (void*) callback
  );
}

void Touchscreen::setObjectTemperatureWriter(TemperatureWriterCallback callback) {
  static lv_timer_t* timer = nullptr;
  if (timer != nullptr) lv_timer_del(timer);
  
  timer = lv_timer_create(
    [](lv_timer_t* timer) {
      auto getTemperature = (TemperatureWriterCallback) timer->user_data;
      float temperature = getTemperature();
      objectTemperatureSmoother->addReading(temperature);
      float smoothedTemperature = objectTemperatureSmoother->smoothedValue();
      lv_msg_send(NEW_OBJECT_TEMPERATURE_MSG, &smoothedTemperature);
    },
    TEMPERATURE_UPDATE_FREQUENCY_MS,
    (void*) callback
  );
}

void Touchscreen::setThermistorTemperatureWriter(TemperatureWriterCallback callback) {
  static lv_timer_t* timer = nullptr;
  if (timer != nullptr) lv_timer_del(timer);

  timer = lv_timer_create(
    [](lv_timer_t* timer) {
      auto getTemperature = (TemperatureWriterCallback) timer->user_data;
      float temperature = getTemperature();
      thermistorTemperatureSmoother->addReading(temperature);
      float smoothedTemperature = thermistorTemperatureSmoother->smoothedValue();
      lv_msg_send(NEW_THERMISTOR_TEMPERATURE_MSG, &smoothedTemperature);
    },
    TEMPERATURE_UPDATE_FREQUENCY_MS,
    (void*) callback
  );
}