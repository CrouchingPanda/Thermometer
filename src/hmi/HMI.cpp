#include "HMI.h"

#include <stdint.h>

#include <Arduino.h>

#include <lvgl.h>

#include "Display.h"
#include "TouchSurface.h"
#include "gui/ui.h"
#include "gui/pub_sub.h"

constexpr uint8_t LVGL_TIMER_HANDLER_PERIOD_MS = 5;

constexpr uint16_t OBJECT_TEMPERATURE_UPDATE_FREQUENCY_MS = 300;
constexpr uint16_t ON_DIE_TEMPERATURE_UPDATE_FREQUENCY_MS = 3000;
constexpr uint16_t ROOM_TEMPERATURE_UPDATE_FREQUENCY_MS = 3000;
constexpr uint16_t BATTERY_TEMPERATURE_UPDATE_FREQUENCY_MS = 5000;
constexpr uint16_t BATTERY_CHARGE_PERCENTAGE_UPDATE_FREQUENCY_MS = 10000;
constexpr uint16_t BATTERY_CHARGING_STATUS_UPDATE_FREQUENCY_MS = 1000;
constexpr uint16_t CALIBRATION_BOARD_CONNECTION_STATUS_UPDATE_FREQUENCY_MS = 1000;
constexpr uint16_t CALIBRATION_THERMISTORS_TEMPERATURE_UPDATE_FREQUENCY_MS = 1000;

static void logging_init() {
  lv_log_register_print_cb(
    [](const char* message) {
      Serial.printf(message);
      Serial.flush();
    }
  );
}

void PublicHMI::begin() {
  display.begin();
  touchSurface.begin();
}

void PublicHMI::showLoadingScreen() {
  display.showLoadingScreen();
}

void PublicHMI::setLoadingPercent(uint8_t percent) {
  display.setLoadingPercent(percent);
}

void PublicHMI::switchFromLoadingScreen() {
  display.switchFromLoadingScreen();
}

void PublicHMI::startLvgl() {
  lv_init();
  logging_init(); // comes first to log any errors in the following set-ups
  display.registerWithLvgl();
  touchSurface.registerWithLvgl();
  ui_init();
  registerDisplayHooksWithLvgl();
}

void PublicHMI::registerDisplayHooksWithLvgl() {
  uint8_t brightness = display.getDefaultBrightness();
  lv_msg_send(NEW_DISPLAY_BRIGHTNESS, &brightness);

  lv_msg_subscribe(
    SET_DISPLAY_BRIGHTNESS,
    [](void* _, lv_msg_t* message) {
      auto display = static_cast<DisplayScreen*>(lv_msg_get_user_data(message));
      auto newBrightness = static_cast<const uint8_t*>(lv_msg_get_payload(message));
      display->setDefaultBrightness(*newBrightness);
    },
    static_cast<void*>(&display)
  );
}

void PublicHMI::onActivity(OnActivityCallback callback) {
  touchSurface.onTouch(callback);
}

void PublicHMI::initLowPowerMode() {
  display.initLowPowerMode();
}

void PublicHMI::initDefaultMode() {
  display.initDefaultPowerMode();
}

void PublicHMI::run() {
  display.run();
  lv_timer_handler_run_in_period(LVGL_TIMER_HANDLER_PERIOD_MS);
}

void PublicHMI::setBatteryPercentageWriter(BatteryPercentageWriter callback) {
  lv_timer_create(
    [](lv_timer_t* timer) {
      auto getBatteryPercentage = reinterpret_cast<BatteryPercentageWriter>(timer->user_data);
      uint8_t percentage = getBatteryPercentage();
      lv_msg_send(NEW_BATTERY_PERCENTAGE, &percentage);
    },
    BATTERY_CHARGE_PERCENTAGE_UPDATE_FREQUENCY_MS,
    reinterpret_cast<void*>(callback)
  );

  uint8_t percentage = callback();
  lv_msg_send(NEW_BATTERY_PERCENTAGE, &percentage);
}

void PublicHMI::setBatteryChargingStatusWriter(BatteryChargingStatusWriter callback) {
  lv_timer_create(
    [](lv_timer_t* timer) {
      auto getBatteryChargingStatus = reinterpret_cast<BatteryPercentageWriter>(timer->user_data);
      bool batteryIsCharging = getBatteryChargingStatus();
      lv_msg_send(NEW_BATTERY_CHARGING_STATUS, &batteryIsCharging);
    },
    BATTERY_CHARGING_STATUS_UPDATE_FREQUENCY_MS,
    reinterpret_cast<void*>(callback)
  );

  bool batteryIsCharging = callback(); 
  lv_msg_send(NEW_BATTERY_CHARGING_STATUS, &batteryIsCharging);
}

void PublicHMI::setEmissivityWriter(EmissivityWriter callback) {
  float emissivity = callback();
  lv_msg_send(NEW_EMISSIVITY_READING, &emissivity);
}

void PublicHMI::setEmissivityReader(EmissivityReader callback) {
  lv_msg_subscribe(
    SET_EMISSIVITY,
    [](void* _, lv_msg_t* message) {
      auto setEmissivity = reinterpret_cast<EmissivityReader>(lv_msg_get_user_data(message));
      auto newEmissivity = static_cast<const float*>(lv_msg_get_payload(message));
      setEmissivity(*newEmissivity);
    },
    reinterpret_cast<void*>(callback)
  );
}

void PublicHMI::setCalibrationBoardConnectionStatusWriter(ConnectionStatusWriter callback) {
  lv_timer_create(
    [](lv_timer_t* timer) {
      auto getConnectionStatus = reinterpret_cast<ConnectionStatusWriter>(timer->user_data);
      bool boardIsConnected = getConnectionStatus();
      lv_msg_send(NEW_CALIBRATION_BOARD_CONNECTION, &boardIsConnected);
    },
    CALIBRATION_BOARD_CONNECTION_STATUS_UPDATE_FREQUENCY_MS,
    reinterpret_cast<void*>(callback)
  );

  bool boardIsConnected = callback();
  lv_msg_send(NEW_CALIBRATION_BOARD_CONNECTION, &boardIsConnected);
}

void PublicHMI::makeTemperatureWriterTimer(TemperatureWriterTimerDTO* dto) {
  lv_timer_create(
    [](lv_timer_t* timer) {
      auto dto = static_cast<TemperatureWriterTimerDTO*>(timer->user_data);
      float temperature = dto->getTemperature();
      lv_msg_send(dto->message, &temperature);
    },
    dto->frequencyMs,
    static_cast<void*>(dto)
  );
}

void PublicHMI::setObjectTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_OBJECT_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = OBJECT_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void PublicHMI::setRoomTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_ROOM_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = ROOM_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void PublicHMI::setOnDieTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_ON_DIE_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = ON_DIE_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void PublicHMI::setBatteryTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_BATTERY_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = BATTERY_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void PublicHMI::setPrimaryThermistorTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_PRIMARY_THERMISTOR_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = CALIBRATION_THERMISTORS_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void PublicHMI::setSecondaryThermistorTemperatureWriter(TemperatureWriter callback) {
  static TemperatureWriterTimerDTO dto = {
    .message = NEW_SECONDARY_THERMISTOR_TEMPERATURE,
    .getTemperature = callback,
    .frequencyMs = CALIBRATION_THERMISTORS_TEMPERATURE_UPDATE_FREQUENCY_MS
  };
  makeTemperatureWriterTimer(&dto);
}

void PublicHMI::setTimeoutWriter(Message message, TimeoutWriter callback) {
  uint8_t timeoutMins = callback();
  lv_msg_send(message, &timeoutMins);
}

void PublicHMI::setLowPowerModeTimeoutWriter(TimeoutWriter callback) {
  setTimeoutWriter(NEW_LOW_POWER_MODE_TIMEOUT, callback);
}

void PublicHMI::setPowerOffTimeoutWriter(TimeoutWriter callback) {
  setTimeoutWriter(NEW_POWER_OFF_TIMEOUT, callback);
}

void PublicHMI::makeTimeoutReaderSubscription(Message message, TimeoutReader callback) {
  lv_msg_subscribe(
    message,
    [](void* _, lv_msg_t* message) {
      auto setTimeout = reinterpret_cast<TimeoutReader>(lv_msg_get_user_data(message));
      auto newTimeout = static_cast<const uint8_t*>(lv_msg_get_payload(message));
      setTimeout(*newTimeout);
    },
    reinterpret_cast<void*>(callback)
  );
}

void PublicHMI::setLowPowerModeTimeoutReader(TimeoutReader callback) {
  makeTimeoutReaderSubscription(SET_LOW_POWER_MODE_TIMEOUT, callback);
}

void PublicHMI::setPowerOffTimeoutReader(TimeoutReader callback) {
  makeTimeoutReaderSubscription(SET_POWER_OFF_TIMEOUT, callback);
}

void PublicHMI::setBrightnessWriter(Message message, BrightnessWriter callback) {
  uint8_t brightness = callback();
  lv_msg_send(message, &brightness);
}

void PublicHMI::setPowerIndicatorOnBrightnessWriter(BrightnessWriter callback) {
  setBrightnessWriter(NEW_POWER_INDICATOR_ON_BRIGHTNESS, callback);
}

void PublicHMI::setPowerIndicatorMaxBreathBrightnessWriter(BrightnessWriter callback) {
  setBrightnessWriter(NEW_POWER_INDICATOR_MAX_BREATH_BRIGHTNESS, callback);
}

void PublicHMI::makeBrightnessReaderSubscription(Message message, BrightnessReader callback) {
  lv_msg_subscribe(
    message,
    [](void* _, lv_msg_t* message) {
      auto setBrightness = reinterpret_cast<BrightnessReader>(lv_msg_get_user_data(message));
      auto newBrightness = static_cast<const uint8_t*>(lv_msg_get_payload(message));
      setBrightness(*newBrightness);
    },
    reinterpret_cast<void*>(callback)
  );
}

void PublicHMI::setPowerIndicatorOnBrightnessReader(BrightnessReader callback) {
  makeBrightnessReaderSubscription(SET_POWER_INDICATOR_ON_BRIGHTNESS, callback);
}

void PublicHMI::setPowerIndicatorMaxBreathBrightnessReader(BrightnessReader callback) {
  makeBrightnessReaderSubscription(SET_POWER_INDICATOR_MAX_BREATH_BRIGHTNESS, callback);
}

void PublicHMI::setObjectTemperatureSmoothingFactorWriter(SmoothingFactorWriter callback) {
  uint8_t smoothingFactor = callback();
  lv_msg_send(NEW_OBJECT_TEMPERATURE_SMOOTHING, &smoothingFactor);
}

void PublicHMI::setObjectTemperatureSmoothingFactorReader(SmoothingFactorReader callback) {
  lv_msg_subscribe(
    SET_OBJECT_TEMPERATURE_SMOOTHER_FACTOR,
    [](void* _, lv_msg_t* message) {
      auto setSmoothingFactor = reinterpret_cast<SmoothingFactorReader>(lv_msg_get_user_data(message));
      auto newSmoothingFactor = static_cast<const uint8_t*>(lv_msg_get_payload(message));
      setSmoothingFactor(*newSmoothingFactor);
    },
    reinterpret_cast<void*>(callback)
  );
}

void PublicHMI::setRoomTemperatureOffsetWriter(TemperatureOffsetWriter callback) {
  float offset = callback();
  lv_msg_send(NEW_ROOM_TEMPERATURE_OFFSET, &offset);
}

void PublicHMI::setRoomTemperatureOffsetReader(TemperatureOffsetReader callback) {
  lv_msg_subscribe(
    SET_ROOM_TEMPERATURE_OFFSET,
    [](void* _, lv_msg_t* message) {
      auto setOffset = reinterpret_cast<TemperatureOffsetReader>(lv_msg_get_user_data(message));
      auto newOffset = static_cast<const float*>(lv_msg_get_payload(message));
      setOffset(*newOffset);
    },
    reinterpret_cast<void*>(callback)
  );
}

PublicHMI HMI = PublicHMI(Display, CapTouchSurface);