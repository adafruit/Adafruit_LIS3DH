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
Adafruit_LIS3DH::Adafruit_LIS3DH(int8_t cspin, SPIClass *theSPI)
    : _cs(-1), _mosi(-1), _miso(-1), _sck(-1), _sensorID(-1) {
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
  if (I2Cinterface) {
    i2c_dev = new Adafruit_I2CDevice(_i2caddr, I2Cinterface);

    if (!i2c_dev->begin()) {
      return false;
    }
  } else if (_cs != -1) {

    // SPIinterface->beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
    if (_sck == -1) {
      spi_dev = new Adafruit_SPIDevice(_cs,
                                       500000,                // frequency
                                       SPI_BITORDER_MSBFIRST, // bit order
                                       SPI_MODE0,             // data mode
                                       SPIinterface);
    } else {
      spi_dev = new Adafruit_SPIDevice(_cs, _sck, _miso, _mosi,
                                       500000,                // frequency
                                       SPI_BITORDER_MSBFIRST, // bit order
                                       SPI_MODE0);            // data mode
    }

    if (!spi_dev->begin()) {
      return false;
    }
  }

  Adafruit_BusIO_Register _chip_id = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_WHOAMI, 1);

  /* Check connection */
  if (getDeviceID() != _wai) {
    /* No LIS3DH detected ... return false */
    // Serial.println(deviceid, HEX);
    return false;
  }
  Adafruit_BusIO_Register _ctrl1 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL1, 1);
  _ctrl1.write(0x07); // enable all axes, normal mode

  // 400Hz rate
  writeDataRate(LIS3DH_DATARATE_400_HZ);

  Adafruit_BusIO_Register _ctrl4 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL4, 1);
  _ctrl4.write(0x88); // High res & BDU enabled

  Adafruit_BusIO_Register _ctrl3 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL3, 1);
  _ctrl3.write(0x10); // DRDY on INT1

  // Turn on orientation config

  Adafruit_BusIO_Register _tmp_cfg = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_TEMPCFG, 1);
  _tmp_cfg.write(0x80); // enable adcs

  return true;
}

/*!
 *  @brief  Sets the data rate for the H3LIS331 (controls power consumption)
 *  @param  dataRate
 *          data rate value
 */
void Adafruit_LIS3DH::setDataRate(lis3dh_dataRate_t dataRate) {

  writeDataRate((lis3dh_dataRate_t)dataRate);
}

/*!
 *   @brief  Gets the data rate for the H3LIS331 (controls power consumption)
 *   @return Returns Data Rate value
 */
lis3dh_dataRate_t Adafruit_LIS3DH::getDataRate(void) {

  return (lis3dh_dataRate_t)readDataRate();
}

void Adafruit_LIS3DH::setRange(lis3dh_range_t range) {
  writeRange((uint8_t)range);
}

/**
 *   @brief  Gets the measurement range for the H3LIS331
 *   @return The range value
 */
lis3dh_range_t Adafruit_LIS3DH::getRange(void) {

  return (lis3dh_range_t)readRange();
}

void Adafruit_LIS3DH::_scaleValues(void) {
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
  adc--; // switch to 0 indexed

  uint16_t value;
  uint8_t reg = LIS3X_REG_OUTADC1_L + (adc * 2);

  if (i2c_dev) {
    reg |= 0x80; // set [7] for auto-increment
  } else {
    reg |= 0x40; // set [6] for auto-increment
    reg |= 0x80; // set [7] for read
  }

  uint8_t buffer[2];
  Adafruit_BusIO_Register adc_data =
      Adafruit_BusIO_Register(i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, reg, 2);

  adc_data.read(buffer, 2);

  value = buffer[0];
  value |= ((uint16_t)buffer[1]) << 8;

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

  Adafruit_BusIO_Register ctrl3 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL3, 1);
  Adafruit_BusIO_RegisterBits i1_click =
      Adafruit_BusIO_RegisterBits(&ctrl3, 1, 7);

  Adafruit_BusIO_Register click_cfg = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CLICKCFG, 1);

  if (!c) {
    // disable int
    i1_click.write(0); // disable i1 click
    click_cfg.write(0);
    return;
  }
  // else...

  i1_click.write(1); // enable i1 click

  Adafruit_BusIO_Register ctrl5 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CTRL5, 1);

  Adafruit_BusIO_RegisterBits int1_latch_bit =
      Adafruit_BusIO_RegisterBits(&ctrl5, 1, 3);
  int1_latch_bit.write(true);

  if (c == 1)
    click_cfg.write(0x15); // turn on all axes & singletap
  if (c == 2)
    click_cfg.write(0x2A); // turn on all axes & doubletap

  Adafruit_BusIO_Register click_ths = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CLICKTHS, 1);
  click_ths.write(clickthresh); // arbitrary

  Adafruit_BusIO_Register time_limit = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_TIMELIMIT, 1);
  time_limit.write(timelimit); // arbitrary

  Adafruit_BusIO_Register time_latency = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_TIMELATENCY, 1);
  time_latency.write(timelatency); // arbitrary

  Adafruit_BusIO_Register time_window = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_TIMEWINDOW, 1);
  time_window.write(timewindow); // arbitrary
}

/*!
 *   @brief  Get uint8_t for single or double click
 *   @return register LIS3X_REG_CLICKSRC
 */
uint8_t Adafruit_LIS3DH::getClick(void) {
  Adafruit_BusIO_Register click_reg = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, LIS3X_REG_CLICKSRC, 1);

  return click_reg.read();
}
