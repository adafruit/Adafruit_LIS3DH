/*!
 *  @file Adafruit_LIS3DH.h
 *
 *  This is a library for the Adafruit LIS3DH Accel breakout board
 *
 *  Designed specifically to work with the Adafruit LIS3DH Triple-Axis
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

#ifndef ADAFRUIT_LIS3DH_H
#define ADAFRUIT_LIS3DH_H

#include "Arduino.h"

#include <SPI.h>
#include <Wire.h>

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_LIS3X.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_Sensor.h>

/** I2C ADDRESS/BITS **/
#define LIS3DH_DEFAULT_ADDRESS (0x18) // if SDO/SA0 is 3V, its 0x19

/** A structure to represent scales **/
typedef enum {
  LIS3DH_RANGE_16_G = 0b11, // +/- 16g
  LIS3DH_RANGE_8_G = 0b10,  // +/- 8g
  LIS3DH_RANGE_4_G = 0b01,  // +/- 4g
  LIS3DH_RANGE_2_G = 0b00,  // +/- 2g (default value)
} lis3dh_range_t;

/** Used with register 0x2A (LIS3X_REG_CTRL_REG1) to set bandwidth **/
typedef enum {
  LIS3DH_DATARATE_400_HZ = 0b01110, //  400Hz
  LIS3DH_DATARATE_200_HZ = 0b01100, //  200Hz
  LIS3DH_DATARATE_100_HZ = 0b01010, //  100Hz
  LIS3DH_DATARATE_50_HZ = 0b01000,  //   50Hz
  LIS3DH_DATARATE_25_HZ = 0b00110,  //   25Hz
  LIS3DH_DATARATE_10_HZ = 0b00100,  // 10 Hz
  LIS3DH_DATARATE_1_HZ = 0b00010,   // 1 Hz
  LIS3DH_DATARATE_POWERDOWN = 00,
  LIS3DH_DATARATE_LOWPOWER_1K6HZ = 0b10000,
  LIS3DH_DATARATE_LOWPOWER_5KHZ = 0b10010,

} lis3dh_dataRate_t;

/*!
 *  @brief  Class that stores state and functions for interacting with
 *          Adafruit_LIS3DH
 */
class Adafruit_LIS3DH : public Adafruit_LIS3X {
public:
  Adafruit_LIS3DH(TwoWire *Wi = &Wire);
  Adafruit_LIS3DH(int8_t cspin, SPIClass *theSPI = &SPI);
  Adafruit_LIS3DH(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin);

  bool begin(uint8_t addr = LIS3DH_DEFAULT_ADDRESS, uint8_t nWAI = 0x33);

  int16_t readADC(uint8_t a);
  void setClick(uint8_t c, uint8_t clickthresh, uint8_t timelimit = 10,
                uint8_t timelatency = 20, uint8_t timewindow = 255);
  uint8_t getClick(void);

  void setDataRate(lis3dh_dataRate_t dataRate);
  lis3dh_dataRate_t getDataRate(void);

  void setRange(lis3dh_range_t range);
  lis3dh_range_t getRange(void);

  // protected:
  // float x_g; /**< x_g axis value (calculated by selected range) */
  // float y_g; /**< y_g axis value (calculated by selected range) */
  // float z_g; /**< z_g axis value (calculated by selected scale) */

private:
  void _scaleValues(void);

  TwoWire *I2Cinterface;
  SPIClass *SPIinterface;

  uint8_t _wai;

  int8_t _cs, _mosi, _miso, _sck;

  int8_t _i2caddr;

  int32_t _sensorID;
};

#endif
