#include "HMI.h"

#include <Arduino.h>

#include <lvgl.h>

#include "Display.h"
#include "TouchSurface.h"
#include "gui/ui.h"
#include "gui/pub_sub.h"

static void logging_init() {
  lv_log_register_print_cb(
    [](const char* message) {
      Serial.printf(message);
      Serial.flush();
    }
  );
}

void HMI::begin() {
  display->begin();
  touchSurface->begin();
}

void HMI::showLoadingScreen() {
  display->showLoadingScreen();
}

void HMI::setLoadingPercent(uint8_t percent) {
  display->setLoadingPercent(percent);
}

void HMI::switchFromLoadingScreen() {
  display->switchFromLoadingScreen();
}

void HMI::startLvgl() {
  lv_init();
  logging_init(); // comes first to log any errors in the following set-ups
  display->registerWithLvgl();
  touchSurface->registerWithLvgl();
  ui_init();
  registerDisplayHooksWithLvgl();
}

void HMI::registerDisplayHooksWithLvgl() {
  uint8_t brightness = display->getDefaultBrightness();
  lv_msg_send(NEW_DISPLAY_BRIGHTNESS, &brightness);

  lv_msg_subscribe(
    SET_DISPLAY_BRIGHTNESS,
    [](void* _, lv_msg_t* message) {
      uint8_t* newBrightness = (uint8_t*) lv_msg_get_payload(message);
      display->setDefaultBrightness(*newBrightness);
    },
    (void*) nullptr
  );
}

void HMI::onActivity(OnActivityCallback callback) {
  touchSurface->onTouch(callback);
}

void HMI::initLowPowerMode() {
  display->initLowPowerMode();
}

void HMI::initDefaultMode() {
  display->initDefaultPowerMode();
}

void HMI::run() {
  display->run();
  lv_timer_handler_run_in_period(LVGL_TIMER_HANDLER_PERIOD_MS);
}

void HMI::setBatteryPercentageWriter(BatteryPercentageWriter callback) {
  lv_timer_create(
    [](lv_timer_t* timer) {
      auto getBatteryPercentage = (BatteryPercentageWriter) timer->user_data;
      uint8_t percentage = getBatteryPercentage();
      lv_msg_send(NEW_BATTERY_PERCENTAGE, &percentage);
    },
    BATTERY_CHARGE_PERCENTAGE_UPDATE_FREQUENCY_MS,
    (void*) callback
  );

  uint8_t percentage = callback();
  lv_msg_send(NEW_BATTERY_PERCENTAGE, &percentage);
}

void HMI::setBatteryChargingStatusWriter(BatteryChargingStatusWriter callback) {
  lv_timer_create(
    [](lv_timer_t* timer) {
      auto getBatteryChargingStatus = (BatteryPercentageWriter) timer->user_data;
      bool batteryIsCharging = getBatteryChargingStatus();
      lv_msg_send(NEW_BATTERY_CHARGING_STATUS, &batteryIsCharging);
    },
    BATTERY_CHARGING_STATUS_UPDATE_FREQUENCY_MS,
    (void*) callback
  );

  bool batteryIsCharging = callback(); 
  lv_msg_send(NEW_BATTERY_CHARGING_STATUS, &batteryIsCharging);
}

void HMI::setEmissivityWriter(EmissivityWriter callback) {
  float emissivity = callback();
  lv_msg_send(NEW_EMISSIVITY_READING, &emissivity);
}

void HMI::setEmissivityReader(EmissivityReader callback) {
  lv_msg_subscribe(
    SET_EMISSIVITY,
    [](void* _, lv_msg_t* message) {
      auto setEmissivity = (EmissivityReader) lv_msg_get_user_data(message);
      float* newEmissivity = (float*) lv_msg_get_payload(message);
      setEmissivity(*newEmissivity);
    },
    (void*) callback
  );
}

void HMI::setCalibrationBoardConnectionStatusWriter(ConnectionStatusWriter  callback) {
  lv_timer_create(
    [](lv_timer_t* timer) {
      auto getConnectionStatus = (ConnectionStatusWriter) timer->user_data;
      bool boardIsConnected = getConnectionStatus();
      lv_msg_send(NEW_CALIBRATION_BOARD_CONNECTION, &boardIsConnected);
    },
    CALIBRATION_BOARD_CONNECTION_STATUS_UPDATE_FREQUENCY_MS,
    (void*) callback
  );

  bool boardIsConnected = callback();
  lv_msg_send(NEW_CALIBRATION_BOARD_CONNECTION, &boardIsConnected);
}

void HMI::makeTemperatureWriterTimer(TemperatureWriterTimerDTO* dto) {
  lv_timer_create(
    [](lv_timer_t* timer) {
      auto dto = (TemperatureWriterTimerDTO*) timer->user_data;
      float temperature = dto->getTemperature();
      lv_msg_send(dto->message, &temperature);
    },
    dto->frequencyMs,
    (void*) dto
  );
}

void HMI::setObjectTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_OBJECT_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = OBJECT_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void HMI::setRoomTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_ROOM_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = ROOM_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void HMI::setOnDieTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_ON_DIE_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = ON_DIE_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void HMI::setBatteryTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_BATTERY_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = BATTERY_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void HMI::setPrimaryThermistorTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_PRIMARY_THERMISTOR_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = CALIBRATION_THERMISTORS_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void HMI::setSecondaryThermistorTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_SECONDARY_THERMISTOR_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = CALIBRATION_THERMISTORS_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void HMI::setTimeoutWriter(Message message, TimeoutWriter callback) {
  uint8_t timeoutMins = callback();
  lv_msg_send(message, &timeoutMins);
}

void HMI::setLowPowerModeTimeoutWriter(TimeoutWriter callback) {
  setTimeoutWriter(NEW_LOW_POWER_MODE_TIMEOUT, callback);
}

void HMI::setPowerOffTimeoutWriter(TimeoutWriter callback) {
  setTimeoutWriter(NEW_POWER_OFF_TIMEOUT, callback);
}

void HMI::makeTimeoutReaderSubscription(Message message, TimeoutReader callback) {
  lv_msg_subscribe(
    message,
    [](void* _, lv_msg_t* message) {
      auto setTimeout = (TimeoutReader) lv_msg_get_user_data(message);
      uint8_t* newTimeout = (uint8_t*) lv_msg_get_payload(message);
      setTimeout(*newTimeout);
    },
    (void*) callback
  );
}

void HMI::setLowPowerModeTimeoutReader(TimeoutReader callback) {
  makeTimeoutReaderSubscription(SET_LOW_POWER_MODE_TIMEOUT, callback);
}

void HMI::setPowerOffTimeoutReader(TimeoutReader callback) {
  makeTimeoutReaderSubscription(SET_POWER_OFF_TIMEOUT, callback);
}

void HMI::setBrightnessWriter(Message message, BrightnessWriter callback) {
  uint8_t brightness = callback();
  lv_msg_send(message, &brightness);
}

void HMI::setPowerIndicatorOnBrightnessWriter(BrightnessWriter callback) {
  setBrightnessWriter(NEW_POWER_INDICATOR_ON_BRIGHTNESS, callback);
}

void HMI::setPowerIndicatorMaxBreathBrightnessWriter(BrightnessWriter callback) {
  setBrightnessWriter(NEW_POWER_INDICATOR_MAX_BREATH_BRIGHTNESS, callback);
}

void HMI::makeBrightnessReaderSubscription(Message message, BrightnessReader callback) {
  lv_msg_subscribe(
    message,
    [](void* _, lv_msg_t* message) {
      auto setBrightness = (BrightnessReader) lv_msg_get_user_data(message);
      uint8_t* newBrightness = (uint8_t*) lv_msg_get_payload(message);
      setBrightness(*newBrightness);
    },
    (void*) callback
  );
}

void HMI::setPowerIndicatorOnBrightnessReader(BrightnessReader callback) {
  makeBrightnessReaderSubscription(SET_POWER_INDICATOR_ON_BRIGHTNESS, callback);
}

void HMI::setPowerIndicatorMaxBreathBrightnessReader(BrightnessReader callback) {
  makeBrightnessReaderSubscription(SET_POWER_INDICATOR_MAX_BREATH_BRIGHTNESS, callback);
}

void HMI::setObjectTemperatureSmoothingFactorWriter(SmoothingFactorWriter callback) {
  uint8_t smoothingFactor = callback();
  lv_msg_send(NEW_OBJECT_TEMPERATURE_SMOOTHING, &smoothingFactor);
}

void HMI::setObjectTemperatureSmoothingFactorReader(SmoothingFactorReader callback) {
  lv_msg_subscribe(
    SET_OBJECT_TEMPERATURE_SMOOTHER_FACTOR,
    [](void* _, lv_msg_t* message) {
      auto setSmoothingFactor = (SmoothingFactorReader) lv_msg_get_user_data(message);
      uint8_t* newSmoothingFactor = (uint8_t*) lv_msg_get_payload(message);
      setSmoothingFactor(*newSmoothingFactor);
    },
    (void*) callback
  );
}

void HMI::setRoomTemperatureOffsetWriter(TemperatureOffsetWriter callback) {
  float offset = callback();
  lv_msg_send(NEW_ROOM_TEMPERATURE_OFFSET, &offset);
}

void HMI::setRoomTemperatureOffsetReader(TemperatureOffsetReader callback) {
  lv_msg_subscribe(
    SET_ROOM_TEMPERATURE_OFFSET,
    [](void* _, lv_msg_t* message) {
      auto setOffset = (TemperatureOffsetReader) lv_msg_get_user_data(message);
      float* newOffset = (float*) lv_msg_get_payload(message);
      setOffset(*newOffset);
    },
    (void*) callback
  );
}