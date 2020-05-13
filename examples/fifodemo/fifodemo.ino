
// Basic demo for accelerometer readings via fifo from Adafruit LIS3DH

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
// Used for hardware & software SPI
#define LIS3DH_CS 10

// software SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS, LIS3DH_MOSI, LIS3DH_MISO, LIS3DH_CLK);
// hardware SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS);
// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("LIS3DH test!");

  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = "); Serial.print(2 << lis.getRange());
  Serial.println("G");

  // First set the fifo to bypass mode to clear the FIFO
  // This important because the hardware might not have been reset if we warm flashed
  lis.setFifoMode(LIS3DH_FIFO_BYPASS);

  lis.setDataRate(LIS3DH_DATARATE_100_HZ);
  Serial.print("Data rate set to: ");
  switch (lis.getDataRate()) {
    case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
    case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
    case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
    case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
    case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
    case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
    case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;

    case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
    case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
    case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
  }
  // enable the fifo and set the right mode
  lis.setFifoMode(LIS3DH_FIFO_MODE); // writes will stop once the fifo is full
  //lis.setFifoMode(LIS3DH_FIFO_STREAM); // stream mode will overrwrite fifo values once full
  //lis.setFifoMode(LIS3DH_FIFO_STREAM_TO); // will be in stream mode until an interrupt comes in
}
// A place to store our samples
sensors_event_t data[32]; 
void loop() {
  
  // We are going to get all the data at once
  // The Fifo can hold 32 samples at once
  int samples_collected = 0;
  for (samples_collected=0; samples_collected < 32 && lis.haveNewData(); samples_collected++) {
    lis.getEvent(&data[i]);
    samples_collected = i;
  }
  // We should have 32 samples
  // Process them here

  // We can just wait for a second for the fifo to queue more samples
  // We get 100 samples a second and it can hold 32 samples
  delay(900/(100/32)); // 0.9 seconds / 100 samples / 32 samples in queue = ms to wait until fifo is full
}
