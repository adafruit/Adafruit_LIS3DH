
// Basic demo for accelerometer readings from Adafruit H3LIS331

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_H3LIS331.h>
#include <Adafruit_Sensor.h>

// Used for software SPI
#define H3LIS331_CLK 13
#define H3LIS331_MISO 12
#define H3LIS331_MOSI 11
// Used for hardware & software SPI
#define H3LIS331_CS 10

// software SPI
//Adafruit_H3LIS331 lis = Adafruit_H3LIS331(H3LIS331_CS, H3LIS331_MOSI, H3LIS331_MISO, H3LIS331_CLK);
// hardware SPI
//Adafruit_H3LIS331 lis = Adafruit_H3LIS331(H3LIS331_CS);
// I2C
Adafruit_H3LIS331 lis = Adafruit_H3LIS331();

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("H3LIS331 test!");

  if (! lis.begin_I2C(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("H3LIS331 found!");

//  lis.setRange(H3LIS331_RANGE_100_G);   // 100, 200, or 400 G!
//  // lis.setDataRate(LIS331_DATARATE_50_HZ);
//  Serial.print("Range set to: ");
//  switch (lis.getRange()) {
//    case H3LIS331_RANGE_100_G: Serial.println("100 g"); break;
//    case H3LIS331_RANGE_200_G: Serial.println("200 g"); break;
//    case H3LIS331_RANGE_400_G: Serial.println("400 g"); break;
//  }
  // // lis.setDataRate(LIS331_DATARATE_50_HZ);
  // Serial.print("Data rate set to: ");
  // switch (lis.getDataRate()) {
  //   case LIS331_DATARATE_1_HZ: Serial.println("1 Hz"); break;
  //   case LIS331_DATARATE_10_HZ: Serial.println("10 Hz"); break;
  //   case LIS331_DATARATE_25_HZ: Serial.println("25 Hz"); break;
  //   case LIS331_DATARATE_50_HZ: Serial.println("50 Hz"); break;
  //   case LIS331_DATARATE_100_HZ: Serial.println("100 Hz"); break;
  //   case LIS331_DATARATE_200_HZ: Serial.println("200 Hz"); break;
  //   case LIS331_DATARATE_400_HZ: Serial.println("400 Hz"); break;

  //   case LIS331_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
  //   case LIS331_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
  //   case LIS331_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
  // }
}

void loop() {
  lis.read();      // get X Y and Z data at once

  /* Or....get a new sensor event, normalized */
  sensors_event_t event;
  lis.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
  Serial.print(" \tY: "); Serial.print(event.acceleration.y);
  Serial.print(" \tZ: "); Serial.print(event.acceleration.z);
  Serial.println(" m/s^2 ");

  Serial.println();

  delay(1000);
}