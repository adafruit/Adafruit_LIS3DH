/*!
 *  @file Adafruit_H3LIS331.h
 *
 *  This is a library for the Adafruit H3LIS331 Accel breakout board
 *
 *  Designed specifically to work with the Adafruit H3LIS331 Triple-Axis
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

#ifndef ADAFRUIT_H3LIS331_H
#define ADAFRUIT_H3LIS331_H

#include "Arduino.h"

#include <SPI.h>
#include <Wire.h>

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_Sensor.h>

/** I2C ADDRESS/BITS **/
#define H3LIS331_DEFAULT_ADDRESS (0x18) // if SDO/SA0 is 3V, its 0x19

/*!
 *  STATUS_REG_AUX register
 *   321OR  1, 2 and 3 axis data overrun. Default value: 0
 *          (0: no overrun has occurred; 1: a new set of data has overwritten
 * the previous ones) 3OR    3 axis data overrun. Default value: 0 (0: no
 * overrun has occurred; 1: a new data for the 3-axis has overwritten the
 * previous one) 2OR    2 axis data overrun. Default value: 0 (0: no overrun has
 * occurred; 1: a new data for the 4-axis has overwritten the previous one) 1OR
 * 1 axis data overrun. Default value: 0 (0: no overrun has occurred; 1: a new
 * data for the 1-axis has overwritten the previous one) 321DA  1, 2 and 3 axis
 * new data available. Default value: 0 (0: a new set of data is not yet
 * available; 1: a new set of data is available) 3DA:   3 axis new data
 * available. Default value: 0 (0: a new data for the 3-axis is not yet
 * available; 1: a new data for the 3-axis is available) 2DA:   2 axis new data
 * available. Default value: 0 (0: a new data for the 2-axis is not yet
 * available; 1: a new data for the 2-axis is available) 1DA    1 axis new data
 * available. Default value: 0 (0: a new data for the 1-axis is not yet
 * available; 1: a new data for the 1-axis is available)
 */
#define H3LIS331_REG_STATUS1 0x07
#define H3LIS331_REG_OUTADC1_L 0x08 /**< 1-axis acceleration data. Low value */
#define H3LIS331_REG_OUTADC1_H 0x09 /**< 1-axis acceleration data. High value */
#define H3LIS331_REG_OUTADC2_L 0x0A /**< 2-axis acceleration data. Low value */
#define H3LIS331_REG_OUTADC2_H 0x0B /**< 2-axis acceleration data. High value */
#define H3LIS331_REG_OUTADC3_L 0x0C /**< 3-axis acceleration data. Low value */
#define H3LIS331_REG_OUTADC3_H 0x0D /**< 3-axis acceleration data. High value */
#define H3LIS331_REG_INTCOUNT                                                    \
  0x0E /**< INT_COUNTER register [IC7, IC6, IC5, IC4, IC3, IC2, IC1, IC0] */
#define H3LIS331_REG_WHOAMI                                                      \
  0x0F /**< Device identification register. [0, 0, 1, 1, 0, 0, 1, 1] */
#define H3LIS331_CHIP_ID 0x32
/*!
 *  TEMP_CFG_REG
 *  Temperature configuration register.
 *   ADC_PD   ADC enable. Default value: 0
 *            (0: ADC disabled; 1: ADC enabled)
 *   TEMP_EN  Temperature sensor (T) enable. Default value: 0
 *            (0: T disabled; 1: T enabled)
 */
#define H3LIS331_REG_TEMPCFG 0x1F
/*!
 *  CTRL_REG1
 *  [ODR3, ODR2, ODR1, ODR0, LPen, Zen, Yen, Xen]
 *   ODR3-0  Data rate selection. Default value: 00
 *           (0000:50 Hz; Others: Refer to Datasheet Table 26, “Data rate
 * configuration”) LPen    Low power mode enable. Default value: 0 (0: normal
 * mode, 1: low power mode) Zen     Z axis enable. Default value: 1 (0: Z axis
 * disabled; 1: Z axis enabled) Yen     Y axis enable. Default value: 1 (0: Y
 * axis disabled; 1: Y axis enabled) Xen     X axis enable. Default value: 1 (0:
 * X axis disabled; 1: X axis enabled)
 */
#define H3LIS331_REG_CTRL1 0x20
/*!
 *  CTRL_REG2
 *  [HPM1, HPM0, HPCF2, HPCF1, FDS, HPCLICK, HPIS2, HPIS1]
 *   HPM1-0  High pass filter mode selection. Default value: 00
 *           Refer to Table 29, "High pass filter mode configuration"
 *   HPCF2-1 High pass filter cut off frequency selection
 *   FDS     Filtered data selection. Default value: 0
 *					 (0: internal filter bypassed; 1: data
 *from internal filter sent to output register and FIFO) HPCLICK High pass
 *filter enabled for CLICK function. (0: filter bypassed; 1: filter enabled)
 *HPIS2   X axis enable. Default value: 1 (0: X axis disabled; 1: X axis
 *enabled) HPIS1 High pass filter enabled for AOI function on interrupt 1, (0:
 *filter bypassed; 1: filter enabled)
 */
#define H3LIS331_REG_CTRL2 0x21
/*!
 *  CTRL_REG3
 *  [I1_CLICK, I1_AOI1, I1_AOI2, I1_DRDY1, I1_DRDY2, I1_WTM, I1_OVERRUN, --]
 *   I1_CLICK    CLICK interrupt on INT1. Default value 0.
 *						   (0: Disable; 1: Enable)
 *   I1_AOI1     AOI1 interrupt on INT1. Default value 0.
 *						   (0: Disable; 1: Enable)
 *   I1_AOI2     AOI2 interrupt on INT1. Default value 0.
 *               (0: Disable; 1: Enable)
 *   I1_DRDY1    DRDY1 interrupt on INT1. Default value 0.
 *               (0: Disable; 1: Enable)
 *   I1_DRDY2    DRDY2 interrupt on INT1. Default value 0.
 *               (0: Disable; 1: Enable)
 *   I1_WTM      FIFO Watermark interrupt on INT1. Default value 0.
 *               (0: Disable; 1: Enable)
 *   I1_OVERRUN  FIFO Overrun interrupt on INT1. Default value 0.
 * 							 (0: Disable; 1: Enable)
 */
#define H3LIS331_REG_CTRL3 0x22
/*!
 *  CTRL_REG4
 *  [BDU, BLE, FS1, FS0, HR, ST1, ST0, SIM]
 *   BDU      Block data update. Default value: 0
 *            (0: continuos update; 1: output registers not updated until MSB
 * and LSB reading) BLE      Big/little endian data selection. Default value 0.
 *            (0: Data LSB @ lower address; 1: Data MSB @ lower address)
 *   FS1-FS0  Full scale selection. default value: 00
 *            (00: +/- 2G; 01: +/- 4G; 10: +/- 8G; 11: +/- 16G)
 *   HR       High resolution output mode: Default value: 0
 *            (0: High resolution disable; 1: High resolution Enable)
 *   ST1-ST0  Self test enable. Default value: 00
 *            (00: Self test disabled; Other: See Table 34)
 *   SIM      SPI serial interface mode selection. Default value: 0
 *            (0: 4-wire interface; 1: 3-wire interface).
 */
#define H3LIS331_REG_CTRL4 0x23
/*!
 *  CTRL_REG5
 *  [BOOT, FIFO_EN, --, --, LIR_INT1, D4D_INT1, 0, 0]
 *   BOOT     Reboot memory content. Default value: 0
 *            (0: normal mode; 1: reboot memory content)
 *   FIFO_EN  FIFO enable. Default value: 0
 *            (0: FIFO disable; 1: FIFO Enable)
 *   LIR_INT1 Latch interrupt request on INT1_SRC register, with INT1_SRC
 * register cleared by reading INT1_SRC itself. Default value: 0. (0: interrupt
 * request not latched; 1: interrupt request latched) D4D_INT1 4D enable: 4D
 * detection is enabled on INT1 when 6D bit on INT1_CFG is set to 1.
 */
#define H3LIS331_REG_CTRL5 0x24

/*!
 *  CTRL_REG6
 *  [I2_CLICKen, I2_INT1, 0, BOOT_I1, 0, --, H_L, -]
 */
#define H3LIS331_REG_CTRL6 0x25
#define H3LIS331_REG_REFERENCE 0x26 /**< REFERENCE/DATACAPTURE **/
/*!
 *  STATUS_REG
 *  [ZYXOR, ZOR, YOR, XOR, ZYXDA, ZDA, YDA, XDA]
 *   ZYXOR    X, Y and Z axis data overrun. Default value: 0
 *            (0: no overrun has occurred; 1: a new set of data has overwritten
 * the previous ones) ZOR      Z axis data overrun. Default value: 0 (0: no
 * overrun has occurred; 1: a new data for the Z-axis has overwritten the
 * previous one) YOR      Y axis data overrun. Default value: 0 (0: no overrun
 * has occurred;  1: a new data for the Y-axis has overwritten the previous one)
 *   XOR      X axis data overrun. Default value: 0
 *            (0: no overrun has occurred; 1: a new data for the X-axis has
 * overwritten the previous one) ZYXDA    X, Y and Z axis new data available.
 * Default value: 0 (0: a new set of data is not yet available; 1: a new set of
 * data is available) ZDA      Z axis new data available. Default value: 0 (0: a
 * new data for the Z-axis is not yet available; 1: a new data for the Z-axis is
 * available) YDA      Y axis new data available. Default value: 0 (0: a new
 * data for the Y-axis is not yet available; 1: a new data for the Y-axis is
 * available)
 */
#define H3LIS331_REG_STATUS2 0x27
#define H3LIS331_REG_OUT_X_L 0x28 /**< X-axis acceleration data. Low value */
#define H3LIS331_REG_OUT_X_H 0x29 /**< X-axis acceleration data. High value */
#define H3LIS331_REG_OUT_Y_L 0x2A /**< Y-axis acceleration data. Low value */
#define H3LIS331_REG_OUT_Y_H 0x2B /**< Y-axis acceleration data. High value */
#define H3LIS331_REG_OUT_Z_L 0x2C /**< Z-axis acceleration data. Low value */
#define H3LIS331_REG_OUT_Z_H 0x2D /**< Z-axis acceleration data. High value */
/*!
 *  FIFO_CTRL_REG
 *  [FM1, FM0, TR, FTH4, FTH3, FTH2, FTH1, FTH0]
 *   FM1-FM0  FIFO mode selection. Default value: 00 (see Table 44)
 *   TR       Trigger selection. Default value: 0
 *            0: Trigger event liked to trigger signal on INT1
 *            1: Trigger event liked to trigger signal on INT2
 *   FTH4:0   Default value: 0
 */
#define H3LIS331_REG_FIFOCTRL 0x2E
#define H3LIS331_REG_FIFOSRC                                                     \
  0x2F /**< FIFO_SRC_REG [WTM, OVRN_FIFO, EMPTY, FSS4, FSS3, FSS2, FSS1, FSS0] \
        */
/*!
 *  INT1_CFG
 *  [AOI, 6D, ZHIE/ZUPE, ZLIE/ZDOWNE, YHIE/YUPE, XHIE/XUPE, XLIE/XDOWNE]
 *   AOI         And/Or combination of Interrupt events. Default value: 0. Refer
 * to Datasheet Table 48, "Interrupt mode" 6D          6 direction detection
 * function enabled. Default value: 0. Refer to Datasheet Table 48, "Interrupt
 * mode" ZHIE/ZUPE   Enable interrupt generation on Z high event or on Direction
 * recognition. Default value: 0. (0: disable interrupt request; 1: enable
 * interrupt request) ZLIE/ZDOWNE Enable interrupt generation on Z low event or
 * on Direction recognition. Default value: 0. YHIE/YUPE   Enable interrupt
 * generation on Y high event or on Direction recognition. Default value: 0. (0:
 * disable interrupt request; 1: enable interrupt request.) YLIE/YDOWNE Enable
 * interrupt generation on Y low event or on Direction recognition. Default
 * value: 0. (0: disable interrupt request; 1: enable interrupt request.)
 *   XHIE/XUPE   Enable interrupt generation on X high event or on Direction
 * recognition. Default value: 0. (0: disable interrupt request; 1: enable
 * interrupt request.) XLIE/XDOWNE Enable interrupt generation on X low event or
 * on Direction recognition. Default value: 0. (0: disable interrupt request; 1:
 * enable interrupt request.)
 */
#define H3LIS331_REG_INT1CFG 0x30
/*!
 *  INT1_SRC
 *   [0, IA, ZH, ZL, YH, YL, XH, XL]
 *    IA  Interrupt active. Default value: 0
 *        (0: no interrupt has been generated; 1: one or more interrupts have
 * been generated) ZH  Z high. Default value: 0 (0: no interrupt, 1: Z High
 * event has occurred) ZL  Z low. Default value: 0 (0: no interrupt; 1: Z Low
 * event has occurred) YH  Y high. Default value: 0 (0: no interrupt, 1: Y High
 * event has occurred) YL  Y low. Default value: 0 (0: no interrupt, 1: Y Low
 * event has occurred) XH  X high. Default value: 0 (0: no interrupt, 1: X High
 * event has occurred) XL  X low. Default value: 0 (0: no interrupt, 1: X Low
 * event has occurred)
 *
 *    Interrupt 1 source register. Read only register.
 *    Reading at this address clears INT1_SRC IA bit (and the interrupt signal
 * on INT 1 pin) and allows the refreshment of data in the INT1_SRC register if
 * the latched option  was chosen.
 */
#define H3LIS331_REG_INT1SRC 0x31
#define H3LIS331_REG_INT1THS                                                     \
  0x32 /**< INT1_THS register [0, THS6, THS5, THS4, THS3, THS1, THS0] */
#define H3LIS331_REG_INT1DUR                                                     \
  0x33 /**< INT1_DURATION [0, D6, D5, D4, D3, D2, D1, D0] */
/*!
 *  CLICK_CFG
 *   [--, --, ZD, ZS, YD, YS, XD, XS]
 *   ZD  Enable interrupt double CLICK-CLICK on Z axis. Default value: 0
 *       (0: disable interrupt request;
 *        1: enable interrupt request on measured accel. value higher than
 * preset threshold) ZS  Enable interrupt single CLICK-CLICK on Z axis. Default
 * value: 0 (0: disable interrupt request; 1: enable interrupt request on
 * measured accel. value higher than preset threshold) YD  Enable interrupt
 * double CLICK-CLICK on Y axis. Default value: 0 (0: disable interrupt request;
 *        1: enable interrupt request on measured accel. value higher than
 * preset threshold) YS  Enable interrupt single CLICK-CLICK on Y axis. Default
 * value: 0 (0: disable interrupt request; 1: enable interrupt request on
 * measured accel. value higher than preset threshold) XD  Enable interrupt
 * double CLICK-CLICK on X axis. Default value: 0 (0: disable interrupt request;
 * 1: enable interrupt request on measured accel. value higher than preset
 * threshold) XS  Enable interrupt single CLICK-CLICK on X axis. Default value:
 * 0 (0: disable interrupt request; 1: enable interrupt request on measured
 * accel. value higher than preset threshold)
 */
#define H3LIS331_REG_CLICKCFG 0x38
/*!
 *  CLICK_SRC
 *   [-, IA, DCLICK, SCLICK, Sign, Z, Y, X]
 *   IA  Interrupt active. Default value: 0
 *       (0: no interrupt has been generated; 1: one or more interrupts have
 * been generated) DCLICK  Double CLICK-CLICK enable. Default value: 0 (0:double
 * CLICK-CLICK detection disable, 1: double CLICK-CLICK detection enable) SCLICK
 * Single CLICK-CLICK enable. Default value: 0 (0:Single CLICK-CLICK detection
 * disable, 1: single CLICK-CLICK detection enable) Sign    CLICK-CLICK Sign.
 *           (0: positive detection, 1: negative detection)
 *   Z       Z CLICK-CLICK detection. Default value: 0
 *           (0: no interrupt, 1: Z High event has occurred)
 *   Y       Y CLICK-CLICK detection. Default value: 0
 *           (0: no interrupt, 1: Y High event has occurred)
 *   X       X CLICK-CLICK detection. Default value: 0
 *           (0: no interrupt, 1: X High event has occurred)
 */
#define H3LIS331_REG_CLICKSRC 0x39
/*!
 *  CLICK_THS
 *   [-, Ths6, Ths5, Ths4, Ths3, Ths2, Ths1, Ths0]
 *   Ths6-Ths0  CLICK-CLICK threshold. Default value: 000 0000
 */
#define H3LIS331_REG_CLICKTHS 0x3A
/*!
 *  TIME_LIMIT
 *   [-, TLI6, TLI5, TLI4, TLI3, TLI2, TLI1, TLI0]
 *   TLI7-TLI0  CLICK-CLICK Time Limit. Default value: 000 0000
 */
#define H3LIS331_REG_TIMELIMIT 0x3B
/*!
 *  TIME_LATANCY
 *   [-, TLA6, TLIA5, TLA4, TLA3, TLA2, TLA1, TLA0]
 *   TLA7-TLA0  CLICK-CLICK Time Latency. Default value: 000 0000
 */
#define H3LIS331_REG_TIMELATENCY 0x3C
/*!
 *  TIME_WINDOW
 *   [TW7, TW6, TW5, TW4, TW3, TW2, TW1, TW0]
 *   TW7-TW0  CLICK-CLICK Time window
 */
#define H3LIS331_REG_TIMEWINDOW 0x3D

//CHANGED_BS
/** A structure to represent scales **/
typedef enum {
  H3LIS331_RANGE_100_G, ///< +/- 100g
  H3LIS331_RANGE_200_G, ///< +/- 200g
  H3LIS331_RANGE_400_G = 0x03, ///< +/- 400g

} h3lis331dl_range_t;

/** A structure to represent axes **/
typedef enum {
  H3LIS331_AXIS_X = 0x0,
  H3LIS331_AXIS_Y = 0x1,
  H3LIS331_AXIS_Z = 0x2,
} h3lis331dl_axis_t;

//CHANGED_BS
/** Used with register 0x2A (H3LIS331_REG_CTRL_REG1) to set bandwidth **/
typedef enum {
  H3LIS331_DATARATE_POWERDOWN = 0,
  H3LIS331_DATARATE_50_HZ = 0x4,
  H3LIS331_DATARATE_100_HZ = 0x5,
  H3LIS331_DATARATE_400_HZ = 0x6,
  H3LIS331_DATARATE_1000_HZ = 0x7,
  H3LIS331_DATARATE_LOWPOWER_0_5_HZ = 0x8,
  H3LIS331_DATARATE_LOWPOWER_1_HZ = 0xC,
  H3LIS331_DATARATE_LOWPOWER_2_HZ = 0x10,
  H3LIS331_DATARATE_LOWPOWER_5_HZ = 0x14,
  H3LIS331_DATARATE_LOWPOWER_10_HZ = 0x18,
} h3lis331dl_dataRate_t;

/*!
 *  @brief  Class that stores state and functions for interacting with
 *          Adafruit_H3LIS331
 */
class Adafruit_H3LIS331 : public Adafruit_Sensor {
public:
  Adafruit_H3LIS331(TwoWire *Wi = &Wire);
  Adafruit_H3LIS331(int8_t cspin, SPIClass *theSPI = &SPI);
  Adafruit_H3LIS331(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin);

  bool begin(uint8_t addr = H3LIS331_DEFAULT_ADDRESS, uint8_t nWAI = H3LIS331_CHIP_ID);

  uint8_t getDeviceID(void);
  bool haveNewData(void);

  void read(void);

  void setRange(h3lis331dl_range_t range);
  h3lis331dl_range_t getRange(void);

  void setDataRate(h3lis331dl_dataRate_t dataRate);
  h3lis331dl_dataRate_t getDataRate(void);

  bool getEvent(sensors_event_t *event);
  void getSensor(sensor_t *sensor);

  int16_t x; /**< x axis value */
  int16_t y; /**< y axis value */
  int16_t z; /**< z axis value */

  float x_g; /**< x_g axis value (calculated by selected range) */
  float y_g; /**< y_g axis value (calculated by selected range) */
  float z_g; /**< z_g axis value (calculated by selected scale) */

private:
  TwoWire *I2Cinterface;
  SPIClass *SPIinterface;

  Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
  Adafruit_SPIDevice *spi_dev = NULL; ///< Pointer to I2C bus interface

  uint8_t _wai;
  int8_t _cs, _mosi, _miso, _sck;
  int8_t _i2caddr;
  int32_t _sensorID;
};

#endif
