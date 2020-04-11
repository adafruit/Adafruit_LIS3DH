/*!
 * @file Adafruit_LIS331HH.cpp
 *
 *  @mainpage Adafruit H3LIS331 breakout board
 *
 *  @section intro_sec Introduction
 *
 *  This is a library for the Adafruit H3LIS331 Accel breakout board
 *
 *  Designed specifically to work with the Adafruit H3LIS331 Accel breakout
 * board.
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/4XXX
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
 *  Bryan Siepert for Adafruit Industries
 *
 *  @section license License
 *
 *  BSD license, all text above must be included in any redistribution
 */

#include "Arduino.h"

#include <Adafruit_LIS331HH.h>
#include <Wire.h>

/*!
 *  @brief  Instantiates a new H3LIS331 class in I2C
 *  @param  Wi
 *          optional wire object
 */
Adafruit_LIS331HH::Adafruit_LIS331HH(){};

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
bool Adafruit_LIS331HH::begin_I2C(uint8_t i2c_address, TwoWire *wire,
                                  int32_t sensor_id) {
  if (i2c_dev) {
    delete i2c_dev; // remove old interface
  }

  i2c_dev = new Adafruit_I2CDevice(i2c_address, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  uint8_t device_id = getDeviceID();
  /* Check connection */
  if (device_id != LIS331_CHIP_ID) {
    /* No H3LIS331 detected ... return false */
    // Serial.print("Chip ID: 0x");Serial.println(device_id, HEX);
    return false;
  }
  Adafruit_BusIO_Register _ctrl1 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL1, 1);
  _ctrl1.write(0x07); // enable all axes, normal mode

  setDataRate(LIS331HH_DATARATE_1000_HZ);
  setRange(LIS331HH_RANGE_24_G);

  // Adafruit_BusIO_Register _ctrl4 = Adafruit_BusIO_Register(
  //     i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL4, 1);
  // _ctrl4.write(0x88); // High res & BDU enabled

  // Adafruit_BusIO_Register _ctrl3 = Adafruit_BusIO_Register(
  //     i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL3, 1);
  // _ctrl3.write(0x10); // DRDY on INT1

  return true;
}

void Adafruit_LIS331HH::_scaleValues(void) {

  // actually 12 bit but left justified
  x >>= 4;
  y >>= 4;
  z >>= 4;
  uint8_t range = getRange();
  uint16_t scale_max = 1;

  if (range == LIS331HH_RANGE_6_G)
    scale_max = 6;
  if (range == LIS331HH_RANGE_12_G)
    scale_max = 12;
  if (range == LIS331HH_RANGE_24_G)
    scale_max = 24;

  float lsb_value = 2 * scale_max * (float)1 / 4098;

  x_g = ((float)x * lsb_value);
  y_g = ((float)y * lsb_value);
  z_g = ((float)z * lsb_value);
}

/*!
 *  @brief  Sets the data rate for the H3LIS331 (controls power consumption)
 *  @param  dataRate
 *          data rate value
 */
void Adafruit_LIS331HH::setDataRate(lis331hh_dataRate_t dataRate) {

  writeDataRate((uint8_t)dataRate);
}

/*!
 *   @brief  Gets the data rate for the H3LIS331 (controls power consumption)
 *   @return Returns Data Rate value
 */
lis331hh_dataRate_t Adafruit_LIS331HH::getDataRate(void) {

  return (lis331hh_dataRate_t)readDataRate();
}
/**
 *  @brief  Sets the measurement range for the H3LIS331
 *  @param  range The range to set
 */

void Adafruit_LIS331HH::setRange(lis331hh_range_t range) {
  writeRange((uint8_t)range);
}

/**
 *   @brief  Gets the measurement range for the H3LIS331
 *   @return The range value
 */
lis331hh_range_t Adafruit_LIS331HH::getRange(void) {

  return (lis331hh_range_t)readRange();
}