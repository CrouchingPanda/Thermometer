#include "ConfigStore.h"

#include <Arduino.h>

#include <LittleFS.h>

#include <ArduinoJson.h>

void ConfigStore::begin() {
  LittleFS.begin();

  File file = LittleFS.open(CONFIG_FILE, "r");
  if (file) {
    deserializeMsgPack(content, file);
  } else {
    file = LittleFS.open(CONFIG_FILE, "w");
    content.to<JsonObject>();
    serializeMsgPack(content, file);
  }
  file.close();
  
  isInitialized = true;
}

ConfigStore Config = ConfigStore();
