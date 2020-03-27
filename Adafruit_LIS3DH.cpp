/*!
 * @file Adafruit_LIS3DH.cpp
 *
 *  @mainpage Adafruit LIS3DH breakout board
 *
 *  @section intro_sec Introduction
 *
 *  This is a library for the Adafruit LIS3DH Accel breakout board
 *
 *  Designed specifically to work with the Adafruit LIS3DH Accel breakout board.
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

#include <Adafruit_LIS3DH.h>
#include <Wire.h>

/*!
 *  @brief  Instantiates a new LIS3DH class in I2C
 *  @param  Wi
 *          optional wire object
 */
Adafruit_LIS3DH::Adafruit_LIS3DH(TwoWire *Wi)
    : _cs(-1), _mosi(-1), _miso(-1), _sck(-1), _sensorID(-1) {
  I2Cinterface = Wi;
}

/*!
 *   @brief  Instantiates a new LIS3DH class using hardware SPI
 *   @param  cspin
 *           number of CSPIN (Chip Select)
 *   @param  *theSPI
 *           optional parameter contains spi object
 */
Adafruit_LIS3DH::Adafruit_LIS3DH(int8_t cspin, SPIClass *theSPI) {
  _cs = cspin;
  _mosi = -1;
  _miso = -1;
  _sck = -1;
  _sensorID = -1;
  SPIinterface = theSPI;
}

/*!
 *   @brief  Instantiates a new LIS3DH class using software SPI
 *   @param  cspin
 *           number of CSPIN (Chip Select)
 *   @param  mosipin
 *           number of pin used for MOSI (Master Out Slave In))
 *   @param  misopin
 *           number of pin used for MISO (Master In Slave Out)
 *   @param  sckpin
 *           number of pin used for CLK (clock pin)
 */
Adafruit_LIS3DH::Adafruit_LIS3DH(int8_t cspin, int8_t mosipin, int8_t misopin,
                                 int8_t sckpin) {
  _cs = cspin;
  _mosi = mosipin;
  _miso = misopin;
  _sck = sckpin;
  _sensorID = -1;
}

/*!
 *  @brief  Setups the HW (reads coefficients values, etc.)
 *  @param  i2caddr
 *          i2c address (optional, fallback to default)
 *  @param  nWAI
 *          Who Am I register value - defaults to 0x33 (LIS3DH)
 *  @return true if successful
 */
bool Adafruit_LIS3DH::begin(uint8_t i2caddr, uint8_t nWAI) {
  _i2caddr = i2caddr;
  _wai = nWAI;
  if (I2Cinterface){
    i2c_dev = new Adafruit_I2CDevice(_i2caddr, I2Cinterface);

    if (!i2c_dev->begin()) {
      return false;
    }
  } else if (_cs != -1){
    Serial.println("NO SPI YET");
    //  spi_dev = new Adafruit_SPIDevice(cs_pin, sck_pin, miso_pin, mosi_pin,
    //                                1000000,               // frequency
    //                                SPI_BITORDER_MSBFIRST, // bit order
    //                                SPI_MODE0);            // data mode
    // if (!spi_dev->begin()) {
    //   return false;
    // }

  }

  Adafruit_BusIO_Register _chip_id = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_WHOAMI, 1);

  /* Check connection */
  // uint /8_t deviceid = _chip_id.read();
  if (getDeviceID() != _wai) {
    /* No LIS3DH detected ... return false */
    // Serial.println(deviceid, HEX);
    return false;
  }
  Adafruit_BusIO_Register _ctrl1 = Adafruit_BusIO_Register(
    i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_CTRL1, 1);
  _ctrl1.write(0x07); // enable all axes, normal mode

  // 400Hz rate
  setDataRate(LIS3DH_DATARATE_400_HZ);

  Adafruit_BusIO_Register _ctrl4 = Adafruit_BusIO_Register(
    i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_CTRL4, 1);
  _ctrl4.write(0x88);// High res & BDU enabled

  Adafruit_BusIO_Register _ctrl3 = Adafruit_BusIO_Register(
    i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_CTRL3, 1);
  _ctrl3.write(0x10); // DRDY on INT1

  // Turn on orientation config
  // writeRegister8(LIS3DH_REG_PL_CFG, 0x40);

  Adafruit_BusIO_Register _tmp_cfg = Adafruit_BusIO_Register(
    i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_TEMPCFG, 1);
  _tmp_cfg.write(0x80);  // enable adcs

  return true;
}

/*!
 *  @brief  Get Device ID from LIS3DH_REG_WHOAMI
 *  @return WHO AM I value
 */
uint8_t Adafruit_LIS3DH::getDeviceID() {
    Adafruit_BusIO_Register _chip_id = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_WHOAMI, 1);

  return _chip_id.read();
}
/*!
 *  @brief  Check to see if new data available
 *  @return true if there is new data available, false otherwise
 */
bool Adafruit_LIS3DH::haveNewData() {
  // checking ZYXDA in REG_STATUS2 tells us if data available
  return (readRegister8(LIS3DH_REG_STATUS2) & 0x8) >> 3;
}

/*!
 *  @brief  Reads x y z values at once
 */
void Adafruit_LIS3DH::read() {
  if (_cs == -1) {
    // i2c
    I2Cinterface->beginTransmission(_i2caddr);
    I2Cinterface->write(LIS3DH_REG_OUT_X_L | 0x80); // 0x80 for autoincrement
    I2Cinterface->endTransmission();

    I2Cinterface->requestFrom(_i2caddr, 6);
    x = I2Cinterface->read();
    x |= ((uint16_t)I2Cinterface->read()) << 8;
    y = I2Cinterface->read();
    y |= ((uint16_t)I2Cinterface->read()) << 8;
    z = I2Cinterface->read();
    z |= ((uint16_t)I2Cinterface->read()) << 8;
  }
#ifndef __AVR_ATtiny85__
  else {
    if (_sck == -1)
      SPIinterface->beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
    digitalWrite(_cs, LOW);
    spixfer(LIS3DH_REG_OUT_X_L | 0x80 | 0x40); // read multiple, bit 7&6 high

    x = spixfer();
    x |= ((uint16_t)spixfer()) << 8;
    y = spixfer();
    y |= ((uint16_t)spixfer()) << 8;
    z = spixfer();
    z |= ((uint16_t)spixfer()) << 8;

    digitalWrite(_cs, HIGH);
    if (_sck == -1)
      SPIinterface->endTransaction(); // release the SPI bus
  }
#endif
  uint8_t range = getRange();
  uint16_t divider = 1;
  if (range == LIS3DH_RANGE_16_G)
    divider = 1365; // different sensitivity at 16g
  if (range == LIS3DH_RANGE_8_G)
    divider = 4096;
  if (range == LIS3DH_RANGE_4_G)
    divider = 8190;
  if (range == LIS3DH_RANGE_2_G)
    divider = 16380;

  x_g = (float)x / divider;
  y_g = (float)y / divider;
  z_g = (float)z / divider;
}

/*!
 *  @brief  Read the auxilary ADC
 *  @param  adc
 *          adc index. possible values (1, 2, 3).
 *  @return auxilary ADC value
 */
int16_t Adafruit_LIS3DH::readADC(uint8_t adc) {
  if ((adc < 1) || (adc > 3))
    return 0;
  uint16_t value;

  adc--;

  uint8_t reg = LIS3DH_REG_OUTADC1_L + adc * 2;

  if (_cs == -1) {
    // i2c
    I2Cinterface->beginTransmission(_i2caddr);
    I2Cinterface->write(reg | 0x80); // 0x80 for autoincrement
    I2Cinterface->endTransmission();
    I2Cinterface->requestFrom(_i2caddr, 2);
    value = I2Cinterface->read();
    value |= ((uint16_t)I2Cinterface->read()) << 8;
  }
#ifndef __AVR_ATtiny85__
  else {
    if (_sck == -1)
      SPIinterface->beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
    digitalWrite(_cs, LOW);
    spixfer(reg | 0x80 | 0x40); // read multiple, bit 7&6 high

    value = spixfer();
    value |= ((uint16_t)spixfer()) << 8;

    digitalWrite(_cs, HIGH);
    if (_sck == -1)
      SPIinterface->endTransaction(); // release the SPI bus
  }
#endif

  return value;
}

/*!
 *   @brief  Set INT to output for single or double click
 *   @param  c
 *					 0 = turn off I1_CLICK
 *           1 = turn on all axes & singletap
 *					 2 = turn on all axes & doubletap
 *   @param  clickthresh
 *           CLICK threshold value
 *   @param  timelimit
 *           sets time limit (default 10)
 *   @param  timelatency
 *   				 sets time latency (default 20)
 *   @param  timewindow
 *   				 sets time window (default 255)
 */
void Adafruit_LIS3DH::setClick(uint8_t c, uint8_t clickthresh,
                               uint8_t timelimit, uint8_t timelatency,
                               uint8_t timewindow) {
  if (!c) {
    // disable int
    uint8_t r = readRegister8(LIS3DH_REG_CTRL3);
    r &= ~(0x80); // turn off I1_CLICK
    writeRegister8(LIS3DH_REG_CTRL3, r);
    writeRegister8(LIS3DH_REG_CLICKCFG, 0);
    return;
  }
  // else...

  writeRegister8(LIS3DH_REG_CTRL3, 0x80); // turn on int1 click
  writeRegister8(LIS3DH_REG_CTRL5, 0x08); // latch interrupt on int1

  if (c == 1)
    writeRegister8(LIS3DH_REG_CLICKCFG, 0x15); // turn on all axes & singletap
  if (c == 2)
    writeRegister8(LIS3DH_REG_CLICKCFG, 0x2A); // turn on all axes & doubletap

  writeRegister8(LIS3DH_REG_CLICKTHS, clickthresh);    // arbitrary
  writeRegister8(LIS3DH_REG_TIMELIMIT, timelimit);     // arbitrary
  writeRegister8(LIS3DH_REG_TIMELATENCY, timelatency); // arbitrary
  writeRegister8(LIS3DH_REG_TIMEWINDOW, timewindow);   // arbitrary
}

/*!
 *   @brief  Get uint8_t for single or double click
 *   @return register LIS3DH_REG_CLICKSRC
 */
uint8_t Adafruit_LIS3DH::getClick() {
  Adafruit_BusIO_Register click_reg = Adafruit_BusIO_Register(
    i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_CLICKSRC, 1);

  return click_reg.read();
}

/*!
 *   @brief  Sets the g range for the accelerometer
 *   @param  range
 *           range value
 */
void Adafruit_LIS3DH::setRange(lis3dh_range_t range) {

  Adafruit_BusIO_Register _ctrl4 = Adafruit_BusIO_Register(
    i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_CTRL4, 1);

  Adafruit_BusIO_RegisterBits range_bits = Adafruit_BusIO_RegisterBits(&_ctrl4, 2, 4);
  range_bits.write(range);
}

/*!
 *  @brief  Gets the g range for the accelerometer
 *  @return Returns g range value
 */
lis3dh_range_t Adafruit_LIS3DH::getRange() {
  Adafruit_BusIO_Register _ctrl4 = Adafruit_BusIO_Register(
    i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_CTRL4, 1);

  Adafruit_BusIO_RegisterBits range_bits = Adafruit_BusIO_RegisterBits(&_ctrl4, 2, 4);
  return (lis3dh_range_t) range_bits.read();
}

/*!
 *  @brief  Sets the data rate for the LIS3DH (controls power consumption)
 *  @param  dataRate
 *          data rate value
 */
void Adafruit_LIS3DH::setDataRate(lis3dh_dataRate_t dataRate) {
  Adafruit_BusIO_Register _ctrl1 = Adafruit_BusIO_Register(
    i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_CTRL1, 1);
  Adafruit_BusIO_RegisterBits data_rate_bits = Adafruit_BusIO_RegisterBits(&_ctrl1, 4, 4);

  data_rate_bits.write(dataRate);
}

/*!
 *   @brief  Gets the data rate for the LIS3DH (controls power consumption)
 *   @return Returns Data Rate value
 */
lis3dh_dataRate_t Adafruit_LIS3DH::getDataRate() {
  Adafruit_BusIO_Register _ctrl1 = Adafruit_BusIO_Register(
    i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3DH_REG_CTRL1, 1);
  Adafruit_BusIO_RegisterBits data_rate_bits = Adafruit_BusIO_RegisterBits(&_ctrl1, 4, 4);

  return (lis3dh_dataRate_t)data_rate_bits.read();
}

/*!
 *  @brief  Gets the most recent sensor event
 *  @param  *event
 *          sensor event that we want to read
 *  @return true if successful
 */
bool Adafruit_LIS3DH::getEvent(sensors_event_t *event) {
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
void Adafruit_LIS3DH::getSensor(sensor_t *sensor) {
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy(sensor->name, "LIS3DH", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name) - 1] = 0;
  sensor->version = 1;
  sensor->sensor_id = _sensorID;
  sensor->type = SENSOR_TYPE_ACCELEROMETER;
  sensor->min_delay = 0;
  sensor->max_value = 0;
  sensor->min_value = 0;
  sensor->resolution = 0;
}

/*!
 *  @brief  Low level SPI
 *  @param  x
 *          value that will be written throught SPI
 *  @return reply
 */
uint8_t Adafruit_LIS3DH::spixfer(uint8_t x) {
#ifndef __AVR_ATtiny85__
  if (_sck == -1)
    return SPIinterface->transfer(x);

  // software spi
  // Serial.println("Software SPI");
  uint8_t reply = 0;
  for (int i = 7; i >= 0; i--) {
    reply <<= 1;
    digitalWrite(_sck, LOW);
    digitalWrite(_mosi, x & (1 << i));
    digitalWrite(_sck, HIGH);
    if (digitalRead(_miso))
      reply |= 1;
  }
  return reply;
#endif
}

/*!
 *  @brief  Writes 8-bits to the specified destination register
 *  @param  reg
 *          register address
 *  @param  value
 *          value that will be written into selected register
 */
void Adafruit_LIS3DH::writeRegister8(uint8_t reg, uint8_t value) {
  if (_cs == -1) {
    I2Cinterface->beginTransmission((uint8_t)_i2caddr);
    I2Cinterface->write((uint8_t)reg);
    I2Cinterface->write((uint8_t)value);
    I2Cinterface->endTransmission();
  }
#ifndef __AVR_ATtiny85__
  else {
    if (_sck == -1)
      SPIinterface->beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
    digitalWrite(_cs, LOW);
    spixfer(reg & ~0x80); // write, bit 7 low
    spixfer(value);
    digitalWrite(_cs, HIGH);
    if (_sck == -1)
      SPIinterface->endTransaction(); // release the SPI bus
  }
#endif
}

/*!
 *  @brief  Reads 8-bits from the specified register
 *  @param  reg
 *          register address
 *  @return read value
 */
uint8_t Adafruit_LIS3DH::readRegister8(uint8_t reg) {
  uint8_t value;

  if (_cs == -1) {
    I2Cinterface->beginTransmission(_i2caddr);
    I2Cinterface->write((uint8_t)reg);
    I2Cinterface->endTransmission();

    I2Cinterface->requestFrom(_i2caddr, 1);
    value = I2Cinterface->read();
  }
#ifndef __AVR_ATtiny85__
  else {
    if (_sck == -1)
      SPIinterface->beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
    digitalWrite(_cs, LOW);
    spixfer(reg | 0x80); // read, bit 7 high
    value = spixfer(0);
    digitalWrite(_cs, HIGH);
    if (_sck == -1)
      SPIinterface->endTransaction(); // release the SPI bus
  }
#endif
  return value;
}
