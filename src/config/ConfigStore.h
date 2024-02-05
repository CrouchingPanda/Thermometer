#ifndef CONFIG_STORE_H
#define CONFIG_STORE_H

#include <Arduino.h>

#include <LittleFS.h>

#include <ArduinoJson.h>

class ConfigStore {

  static constexpr const char* CONFIG_FILE = "/config.cfg";

  JsonDocument content;

  bool isInitialized = false;

  public:

  ConfigStore() : content(JsonDocument()) {};

  void begin();

  template<typename T>
  T getOrDefault(const char* key, T defaultValue) {
    if (!isInitialized) begin();

    JsonObject object = content.as<JsonObject>();
    JsonVariant value = object[key];
    if (value.isNull()) return defaultValue;
    return value.as<T>();
  };

  template<typename T>
  void set(const char* key, T value) {
    if (!isInitialized) begin();

    JsonObject object = content.as<JsonObject>();

    JsonVariant currentValue = object[key];
    if (!currentValue.isNull() && currentValue == value) return;

    object[key] = value;

    File file = LittleFS.open(CONFIG_FILE, "w");
    serializeMsgPack(content, file);
    file.close();
  };
};

extern ConfigStore Config;

#endif // !CONFIG_STORE_H
