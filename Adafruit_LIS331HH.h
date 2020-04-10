/*!
 *  @file Adafruit_LIS331HH.h
 *
 *  This is a library for the Adafruit LIS331HH Accel breakout board
 *
 *  Designed specifically to work with the Adafruit LIS331HH Triple-Axis
 *Accelerometer
 *	(+-2g/4g/8g/16g)
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/2809
 *
 *	This sensor communicates over I2C or SPI (our library code supports
 *both) so you can share it with a bunch of other sensors on the same I2C bus.
 *  There's an address selection pin so you can have two accelerometers share an
 *I2C bus.
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  K. Townsend / Limor Fried (Ladyada) - (Adafruit Industries).
 *
 *  BSD license, all text above must be included in any redistribution
 */

#ifndef ADAFRUIT_LIS331HH_H
#define ADAFRUIT_LIS331HH_H

#include "Adafruit_LIS3DH.h"

/** I2C ADDRESS/BITS **/
#define LIS331HH_DEFAULT_ADDRESS (0x18) // if SDO/SA0 is 3V, its 0x19
#define LIS331_CHIP_ID 0x32

/** A structure to represent scales **/
typedef enum {
  LIS331HH_RANGE_6_G = 0x0, ///< +/- 6G
  LIS331HH_RANGE_12_G = 0x1, ///< +/- 12G
  LIS331HH_RANGE_24_G = 0x03, ///< +/- 24Gvalue)
} lis331hh_range_t;

/** Used with register 0x2A (LIS331HH_REG_CTRL_REG1) to set bandwidth **/
typedef enum {
  LIS331HH_DATARATE_POWERDOWN = 0,
  LIS331HH_DATARATE_50_HZ = 0x4,
  LIS331HH_DATARATE_100_HZ = 0x5,
  LIS331HH_DATARATE_400_HZ = 0x6,
  LIS331HH_DATARATE_1000_HZ = 0x7,
  LIS331HH_DATARATE_LOWPOWER_0_5_HZ = 0x8,
  LIS331HH_DATARATE_LOWPOWER_1_HZ = 0xC,
  LIS331HH_DATARATE_LOWPOWER_2_HZ = 0x10,
  LIS331HH_DATARATE_LOWPOWER_5_HZ = 0x14,
  LIS331HH_DATARATE_LOWPOWER_10_HZ = 0x18,
} lis331hh_dataRate_t;

/*!
 *  @brief  Class that stores state and functions for interacting with
 *          Adafruit_LIS331HH
 */
class Adafruit_LIS331HH : public Adafruit_LIS3DH {
public:
  Adafruit_LIS331HH();
  Adafruit_LIS331HH(int8_t cspin, SPIClass *theSPI = &SPI);
  Adafruit_LIS331HH(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin);
  // Adafruit_LIS331HH(TwoWire *Wi = &Wire);
  // Adafruit_LIS331HH(int8_t cspin, SPIClass *theSPI = &SPI);
  // Adafruit_LIS331HH(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin);

  // bool begin(uint8_t addr = LIS331HH_DEFAULT_ADDRESS, uint8_t nWAI = LIS331_CHIP_ID);
  bool begin_I2C(uint8_t i2c_addr = LIS331HH_DEFAULT_ADDRESS,
                 TwoWire *wire = &Wire, int32_t sensorID = 0);

  // bool begin_SPI(uint8_t cs_pin, SPIClass *theSPI = &SPI, int32_t sensorID = 0);
  // bool begin_SPI(int8_t cs_pin, int8_t sck_pin, int8_t miso_pin,
                //  int8_t mosi_pin, int32_t sensorID = 0);
  void setDataRate(lis331hh_dataRate_t dataRate);
  lis331hh_dataRate_t getDataRate(void);


  void setRange(lis331hh_range_t range);
  lis331hh_range_t getRange(void);
private:
  void _scaleValues(void);
};

#endif
