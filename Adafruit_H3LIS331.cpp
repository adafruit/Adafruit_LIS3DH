/*!
 * @file Adafruit_H3LIS331.cpp
 *
 *  @mainpage Adafruit H3LIS331 breakout board
 *
 *  @section intro_sec Introduction
 *
 *  This is a library for the Adafruit H3LIS331 Accel breakout board
 *
 *  Designed specifically to work with the Adafruit H3LIS331 Accel breakout board.
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/2809
 *
 *  This sensor communicates over I2C or SPI (our library code supports both) so
 * you can share it with a bunch of other sensors on the same I2C bus.
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  @section author Author
 *
 *  K. Townsend / Limor Fried (Adafruit Industries)
 *
 *  @section license License
 *
 *  BSD license, all text above must be included in any redistribution
 */

#include "Arduino.h"

#include <Adafruit_H3LIS331.h>
#include <Wire.h>

/*!
 *  @brief  Instantiates a new H3LIS331 class in I2C
 *  @param  Wi
 *          optional wire object
 */
Adafruit_H3LIS331::Adafruit_H3LIS331(){};




// /*!
//  *    @brief  Sets up the hardware and initializes hardware SPI
//  *    @param  cs_pin The arduino pin # connected to chip select
//  *    @param  theSPI The SPI object to be used for SPI connections.
//  *    @param  sensor_id
//  *            The user-defined ID to differentiate different sensors
//  *    @return True if initialization was successful, otherwise false.
//  */
// bool Adafruit_H3LIS331::begin_SPI(uint8_t cs_pin, SPIClass *theSPI,
//                                 int32_t sensor_id) {
//   i2c_dev = NULL;

//   if (spi_dev) {
//     delete spi_dev; // remove old interface
//   }
//   spi_dev = new Adafruit_SPIDevice(cs_pin,
//                                    1000000,               // frequency
//                                    SPI_BITORDER_MSBFIRST, // bit order
//                                    SPI_MODE0,             // data mode
//                                    theSPI);
//   if (!spi_dev->begin()) {
//     return false;
//   }

//   return _init(sensor_id);
// }

// /*!
//  *    @brief  Sets up the hardware and initializes software SPI
//  *    @param  cs_pin The arduino pin # connected to chip select
//  *    @param  sck_pin The arduino pin # connected to SPI clock
//  *    @param  miso_pin The arduino pin # connected to SPI MISO
//  *    @param  mosi_pin The arduino pin # connected to SPI MOSI
//  *    @param  sensor_id
//  *            The user-defined ID to differentiate different sensors
//  *    @return True if initialization was successful, otherwise false.
//  */
// bool Adafruit_H3LIS331::begin_SPI(int8_t cs_pin, int8_t sck_pin, int8_t miso_pin,
//                                 int8_t mosi_pin, int32_t sensor_id) {
//   i2c_dev = NULL;

//   if (spi_dev) {
//     delete spi_dev; // remove old interface
//   }
//   spi_dev = new Adafruit_SPIDevice(cs_pin, sck_pin, miso_pin, mosi_pin,
//                                    1000000,               // frequency
//                                    SPI_BITORDER_MSBFIRST, // bit order
//                                    SPI_MODE0);            // data mode
//   if (!spi_dev->begin()) {
//     return false;
//   }
//   return _init(sensor_id);
// }

/*!
 *   @brief  Instantiates a new H3LIS331 class using hardware SPI
 *   @param  cspin
 *           number of CSPIN (Chip Select)
 *   @param  *theSPI
 *           optional parameter contains spi object
 */
// Adafruit_H3LIS331::Adafruit_H3LIS331(int8_t cspin, SPIClass *theSPI) {
//   _cs = cspin;
//   _mosi = -1;
//   _miso = -1;
//   _sck = -1;
//   _sensorID = -1;
//   SPIinterface = theSPI;
// }

// /*!
//  *   @brief  Instantiates a new H3LIS331 class using software SPI
//  *   @param  cspin
//  *           number of CSPIN (Chip Select)
//  *   @param  mosipin
//  *           number of pin used for MOSI (Master Out Slave In))
//  *   @param  misopin
//  *           number of pin used for MISO (Master In Slave Out)
//  *   @param  sckpin
//  *           number of pin used for CLK (clock pin)
//  */
// Adafruit_H3LIS331::Adafruit_H3LIS331(int8_t cspin, int8_t mosipin, int8_t misopin,
//                                  int8_t sckpin) {
//   _cs = cspin;
//   _mosi = mosipin;
//   _miso = misopin;
//   _sck = sckpin;
//   _sensorID = -1;
// }

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  i2c_address
 *            The I2C address to be used.
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @param  sensor_id
 *            The user-defined ID to differentiate different sensors
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_H3LIS331::begin_I2C(uint8_t i2c_address, TwoWire *wire, int32_t sensor_id) {
  if (i2c_dev) {
    delete i2c_dev; // remove old interface
  }

  i2c_dev = new Adafruit_I2CDevice(i2c_address, wire);

  if (!i2c_dev->begin()) {
    return false;
  }


  Adafruit_BusIO_Register _chip_id = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_WHOAMI, 1);

  /* Check connection */
  if (getDeviceID() != H3LIS331_CHIP_ID) {
    /* No H3LIS331 detected ... return false */
    // Serial.println(deviceid, HEX);
    return false;
  }
  Adafruit_BusIO_Register _ctrl1 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_CTRL1, 1);
  _ctrl1.write(0x07); // enable all axes, normal mode

  setDataRate(H3LIS331_DATARATE_1000_HZ);
  setRange(H3LIS331_RANGE_100_G);

  // Adafruit_BusIO_Register _ctrl4 = Adafruit_BusIO_Register(
  //     i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_CTRL4, 1);
  // _ctrl4.write(0x88); // High res & BDU enabled

  // Adafruit_BusIO_Register _ctrl3 = Adafruit_BusIO_Register(
  //     i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_CTRL3, 1);
  // _ctrl3.write(0x10); // DRDY on INT1

  // Turn on orientation config

  // Adafruit_BusIO_Register _tmp_cfg = Adafruit_BusIO_Register(
  //     i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_TEMPCFG, 1);
  // _tmp_cfg.write(0x80); // enable adcs

  return true;
}

void Adafruit_H3LIS331::_scaleValues(void){

  // actually 12 bit but left justified
  x >>=4;
  y >>=4;
  z >>= 4;
  uint8_t range = getRange();
  uint16_t scale_max = 1;
  if (range == H3LIS331_RANGE_100_G)
    scale_max = 100;
  if (range == H3LIS331_RANGE_200_G)
    scale_max = 200;
  if (range == H3LIS331_RANGE_400_G)
    scale_max = 400;

  float lsb_value = 2*scale_max * (float) 1/4098;

  x_g = ((float)x * lsb_value);
  y_g = ((float)y * lsb_value);
  z_g = ((float)z * lsb_value);
}


/*!
 *  @brief  Sets the data rate for the H3LIS331 (controls power consumption)
 *  @param  dataRate
 *          data rate value
 */
void Adafruit_H3LIS331::setDataRate(h3lis331dl_dataRate_t dataRate) {

  writeDataRate((lis3dh_dataRate_t)dataRate);
}

/*!
 *   @brief  Gets the data rate for the H3LIS331 (controls power consumption)
 *   @return Returns Data Rate value
 */
h3lis331dl_dataRate_t Adafruit_H3LIS331::getDataRate(void) {

  return (h3lis331dl_dataRate_t)readDataRate();
}