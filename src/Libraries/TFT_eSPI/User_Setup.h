// The PIO can only be user with Earle Philhower's RP2040 board package:
// https://github.com/earlephilhower/arduino-pico

// PIO SPI allows high SPI clock rates to be used when the processor is over-clocked.
// PIO SPI is "write only" and the TFT_eSPI touch functions are not supported.
// A touch screen could be used with a third party library on different SPI pins.

// This invokes the PIO based SPI interface for the RP2040 processor.
#define RP2040_PIO_SPI

// TFT driver
#define ILI9341_DRIVER

// Pins - the PIO will control MOSI, SCLK and DC pins
// Any digital GPIO pins may be used
// #define TFT_MISO  -1 // MISO is not used or supported
#define TFT_MOSI  19
#define TFT_SCLK  18
#define TFT_CS    20  // Chip select control pin
#define TFT_DC    12  // Data Command control pin
#define TFT_RST   15  // Reset pin (could connect to Arduino RESET pin)

#define SPI_FREQUENCY  62500000