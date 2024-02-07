# Thermometer

Board for temperature measurement built around MLX90614 infrared surface temperature sensor.

## Platform

Arduino core used is Raspberry Pi Pico / RP2040 by Earle F. Philhower, III v3.7.0.

[InterboardSerial](https://github.com/CrouchingPanda/InterboardSerial) custom library is used 
for communicating with the calibration board.

## Libraries

| Title              | Version |
|--------------------|---------|
| lvgl               | 8.3.11  |
| TFT_eSPI           | 2.5.34  |
| Smoothed           | 1.2.0   |
| SerialTransfer     | 3.1.3   |
| NeoPixelConnect    | 1.2.0   |
| JLed               | 4.13.1  |
| Adafruit MLX90614  | 2.1.5   |
| Adafruit LC709203F | 1.3.4   |
| Adafruit FT6206    | 1.1.0   |
| Adafruit BusIO     | 1.15.0  |
| ArduinoJson        | 7.0.3   |
| InterboardSerial   | latest  |

## Theory Highlights

### For any object

```math
ε + R + 𝜏 = 1
```

$ε$ = Emissivity

$R$ = Reflectivity

$𝜏$ = Transmissivity

### For non-transparent object

```math
𝜏 = 0; ε + R = 1
```

### Stefan-Boltzmann Law

```math
M_e = P/A = εσ(T_o^4 - T_a^4)
```

$M_e$ = radiant exitance = defined as $P/A$

$P$ = energy per unit time = power = radiant flux

$A$ = area

$σ$ = Stefan's constant

$T_o$ = temperature of an object

$T_a$ = ambient temperature

### Sensor/object arrangement

```
 sensor (M_s)                                object
+-----------        emitted (M_o)           +------+
|            <==============================|      |
|            <-----------------------------_|      |
+-----------        reflected (M_r)        /|------+
                                          /
                                         /
                                        / ambient 
                                       / to be reflected									   
```

```math
M_s = M_o + M_r = εσ(T_o^4 - T_s^4) + Rσ(T_r^4 - T_s^4)
```

$M_s$ = total sensor exitance

$M_o$ = exitance coming from the object itself

$M_r$ = exitance coming from the surroundings = radiation reflected by the object

$T_o$ = object's temperature

$T_s$ = sensor's temperature (on its die)

$T_r$ = room (ambient, background) temperature

### Application

MLX90614 leverages the above to come up with its temperature readings. It has an element monitoring its own 
temperature, as well as another sensor registering incident energy flux. It uses an emissivity value of `1.00` 
by default. As a note, in the equation above the sensor's firmware assumes $T_s = T_r$.

A way to adjust the calculations for a different emissivity is to get the temperature reading from the device with
its default emissivity, thereby getting the incident flux value. Then use the above formula backward, using an arbitrary
emissivity of choice and now taking the temperature for the unknown variable.

Also, when doing calculations manually on MCU, the $T_s = T_r$ assumption can be tweaked, as it has been noted that 
in real-life scenarios the sensor can have higher temperature (perhaps due to self-heating) than the background.
