/*!
 * @file Adafruit_H3LIS331.cpp
 */

#include "Arduino.h"

#include <Adafruit_H3LIS331.h>
#include <Wire.h>

/*!
 *  @brief  Instantiates a new H3LIS331 class
 */
Adafruit_H3LIS331::Adafruit_H3LIS331(){};

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
bool Adafruit_H3LIS331::begin_I2C(uint8_t i2c_address, TwoWire *wire,
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

  setDataRate(H3LIS331_DATARATE_1000_HZ);
  setRange(H3LIS331_RANGE_100_G);

  // Adafruit_BusIO_Register _ctrl4 = Adafruit_BusIO_Register(
  //     i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL4, 1);
  // _ctrl4.write(0x88); // High res & BDU enabled

  // Adafruit_BusIO_Register _ctrl3 = Adafruit_BusIO_Register(
  //     i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL3, 1);
  // _ctrl3.write(0x10); // DRDY on INT1

  return true;
}

void Adafruit_H3LIS331::_scaleValues(void) {

  // actually 12 bit but left justified
  x >>= 4;
  y >>= 4;
  z >>= 4;
  uint8_t range = getRange();
  uint16_t scale_max = 1;
  if (range == H3LIS331_RANGE_100_G)
    scale_max = 100;
  if (range == H3LIS331_RANGE_200_G)
    scale_max = 200;
  if (range == H3LIS331_RANGE_400_G)
    scale_max = 400;

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
void Adafruit_H3LIS331::setDataRate(h3lis331dl_dataRate_t dataRate) {

  writeDataRate((uint8_t)dataRate);
}

/*!
 *   @brief  Gets the data rate for the H3LIS331 (controls power consumption)
 *   @return Returns Data Rate value
 */
h3lis331dl_dataRate_t Adafruit_H3LIS331::getDataRate(void) {

  return (h3lis331dl_dataRate_t)readDataRate();
}

/**
 *  @brief  Sets the measurement range for the H3LIS331
 *  @param  range The range to set
 */

void Adafruit_H3LIS331::setRange(h3lis331dl_range_t range) {
  writeRange((uint8_t)range);
}

/**
 *   @brief  Gets the measurement range for the H3LIS331
 *   @return The range value
 */
h3lis331dl_range_t Adafruit_H3LIS331::getRange(void) {

  return (h3lis331dl_range_t)readRange();
}