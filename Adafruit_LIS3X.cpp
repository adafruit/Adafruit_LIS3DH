/*!
 * @file Adafruit_LIS3X.cpp
 */

#include "Arduino.h"

#include <Adafruit_LIS3X.h>
#include <Wire.h>

/*!
 *  @brief  Instantiates a new LIS3X class in I2C
 *  @param  Wi
 *          optional wire object
 */
Adafruit_LIS3X::Adafruit_LIS3X(TwoWire *Wi) {}

/*!
 *   @brief  Instantiates a new LIS3X class using hardware SPI
 *   @param  cspin
 *           number of CSPIN (Chip Select)
 *   @param  *theSPI
 *           optional parameter contains spi object
 */
Adafruit_LIS3X::Adafruit_LIS3X(int8_t cspin, SPIClass *theSPI) {}

/*!
 *   @brief  Instantiates a new LIS3X class using software SPI
 *   @param  cspin
 *           number of CSPIN (Chip Select)
 *   @param  mosipin
 *           number of pin used for MOSI (Master Out Slave In))
 *   @param  misopin
 *           number of pin used for MISO (Master In Slave Out)
 *   @param  sckpin
 *           number of pin used for CLK (clock pin)
 */
Adafruit_LIS3X::Adafruit_LIS3X(int8_t cspin, int8_t mosipin, int8_t misopin,
                               int8_t sckpin) {}

/*!
 *  @brief  Get Device ID from LIS3X_REG_WHOAMI
 *  @return WHO AM I value
 */
uint8_t Adafruit_LIS3X::getDeviceID(void) {
  Adafruit_BusIO_Register _chip_id = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_WHOAMI, 1);

  return _chip_id.read();
}
/*!
 *  @brief  Check to see if new data available
 *  @return true if there is new data available, false otherwise
 */
bool Adafruit_LIS3X::haveNewData(void) {
  Adafruit_BusIO_Register status_2 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_STATUS2, 1);
  Adafruit_BusIO_RegisterBits zyx_data_available =
      Adafruit_BusIO_RegisterBits(&status_2, 1, 3);
  return zyx_data_available.read();
}

/*!
 *  @brief  Reads x y z values at once
 */
void Adafruit_LIS3X::read(void) {

  uint8_t register_address = LIS3X_REG_OUT_X_L;
  if (i2c_dev) {
    register_address |= 0x80; // set [7] for auto-increment
  } else {
    register_address |= 0x40; // set [6] for auto-increment
    register_address |= 0x80; // set [7] for read
  }

  Adafruit_BusIO_Register xl_data = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, register_address, 6);

  uint8_t buffer[6];
  xl_data.read(buffer, 6);

  x = buffer[0];
  x |= ((uint16_t)buffer[1]) << 8;
  y = buffer[2];
  y |= ((uint16_t)buffer[3]) << 8;
  z = buffer[4];
  z |= ((uint16_t)buffer[5]) << 8;

  _scaleValues();
}

/**
 * @brief Scales the raw variables based on the current measurement range
 *
 */
void Adafruit_LIS3X::_scaleValues(void) {}

/**
 * @brief Set the measurement range for the sensor
 *
 * @param range The measurement range to set
 */
void Adafruit_LIS3X::writeRange(uint8_t range) {

  Adafruit_BusIO_Register _ctrl4 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL4, 1);

  Adafruit_BusIO_RegisterBits range_bits =
      Adafruit_BusIO_RegisterBits(&_ctrl4, 2, 4);
  range_bits.write(range);
  delay(15); // delay to let new setting settle
}

/**
 * @brief Get the measurement range
 *
 * @return uint8_t The measurement range
 */
uint8_t Adafruit_LIS3X::readRange(void) {
  Adafruit_BusIO_Register _ctrl4 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL4, 1);

  Adafruit_BusIO_RegisterBits range_bits =
      Adafruit_BusIO_RegisterBits(&_ctrl4, 2, 4);

  return (uint8_t)range_bits.read();
}

/*!
 *  @brief  Sets the data rate for the LIS3X (controls power consumption)
 *  @param  dataRate
 *          data rate value
 */
void Adafruit_LIS3X::writeDataRate(uint8_t dataRate) {
  Adafruit_BusIO_Register _ctrl1 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL1, 1);
  Adafruit_BusIO_RegisterBits data_rate_bits =
      // Adafruit_BusIO_RegisterBits(&_ctrl1, 4, 4);
      Adafruit_BusIO_RegisterBits(&_ctrl1, 3, 5); // including LPen bit

  data_rate_bits.write(dataRate);
}

/*!
 *   @brief  Gets the data rate for the LIS3X (controls power consumption)
 *   @return Returns Data Rate value
 */
uint8_t Adafruit_LIS3X::readDataRate(void) {
  Adafruit_BusIO_Register _ctrl1 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL1, 1);
  Adafruit_BusIO_RegisterBits data_rate_bits =
      // Adafruit_BusIO_RegisterBits(&_ctrl1, 4, 4);
      Adafruit_BusIO_RegisterBits(&_ctrl1, 3, 5); // including LPen bit

  return (uint8_t)data_rate_bits.read();
}

/*!
 *  @brief  Gets the most recent sensor event
 *  @param  *event
 *          sensor event that we want to read
 *  @return true if successful
 */
bool Adafruit_LIS3X::getEvent(sensors_event_t *event) {
  /* Clear the event */
  memset(event, 0, sizeof(sensors_event_t));

  event->version = sizeof(sensors_event_t);
  event->sensor_id = _sensorID;
  event->type = SENSOR_TYPE_ACCELEROMETER;
  event->timestamp = 0;

  read();

  event->acceleration.x = x_g * SENSORS_GRAVITY_STANDARD;
  event->acceleration.y = y_g * SENSORS_GRAVITY_STANDARD;
  event->acceleration.z = z_g * SENSORS_GRAVITY_STANDARD;

  return true;
}

/*!
 *   @brief  Gets the sensor_t data
 *   @param  *sensor
 *           sensor that we want to write data into
 */
void Adafruit_LIS3X::getSensor(sensor_t *sensor) {
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy(sensor->name, "LIS3X", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name) - 1] = 0;
  sensor->version = 1;
  sensor->sensor_id = _sensorID;
  sensor->type = SENSOR_TYPE_ACCELEROMETER;
  sensor->min_delay = 0;
  sensor->max_value = 0;
  sensor->min_value = 0;
  sensor->resolution = 0;
}