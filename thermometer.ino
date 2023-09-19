#include <Wire.h>
#include <EEPROM.h>
#include <SPI.h>

#include "src/touchscreen/Touchscreen.h"
#include "src/indicator/HealthIndicator.h"
#include "src/hardware/MelexisThermometer.h"
#include "src/hardware/LipoBattery.h"
#include "src/hardware/Thermistor.h"

/*
 * Fuel gauge board (rated for 400kHz I2C max) uses time slicing while 
 * RP2040 does not support it so I2C rate needs to be decreased for that.
 * Melexis Thermometer's I2C rate range is from 10kHz to 100kHz. Touchscreen
 * works fine at 400kHz so Wire interface is used for that while Wire1 is
 * used by these slower boards.
 */
static constexpr uint16_t WIRE_1_I2C_FREQUENCY = 25'000;

static constexpr uint16_t EEPROM_SIZE = 4096; // RP2040 Earle core specific

auto touchscreen = Touchscreen::getInstance();
auto healthIndicator = HealthIndicator();
auto thermometer = MelexisThermometer();
auto battery = LipoBattery();
auto thermistor = Thermistor();

void setup() {
  healthIndicator.begin();

  thermistor.begin();

  Wire1.setClock(WIRE_1_I2C_FREQUENCY);

  // other devices on the same bus must be on before battery communication starts
  thermometer.begin(&Wire1);
  battery.begin(&Wire1);
  
  EEPROM.begin(EEPROM_SIZE);

  touchscreen.begin(&EEPROM);
  
  touchscreen.setObjectTemperatureWriter(
    [](){ return thermometer.objectTemperatureFarengheit(); }
  );

  touchscreen.setAmbientTemperatureWriter(
    [](){ return thermometer.ambientTemperatureFarengheit(); }
  );

  touchscreen.setThermistorTemperatureWriter(
    [](){ return thermistor.temperatureFarengheit(); }
  );

  touchscreen.setEmissivityWriter(
    [](){ return thermometer.getEmissivity(); }
  );

  touchscreen.setEmissivityReader(
    [](float emissivity){ thermometer.setEmissivity(emissivity); }
  );

  touchscreen.setBatteryPercentageWriter(
    [](){ return battery.chargePercentage(); }
  );

  touchscreen.setBatteryChargingStatusWriter(
    [](){ return battery.isCharging(); }
  );
}

void loop() {
  touchscreen.run();
  healthIndicator.run();
}