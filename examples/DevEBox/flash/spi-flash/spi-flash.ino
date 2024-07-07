#include "FlashW25QXX.h"
#include "SPI.h"

FlashW25QXX flash;

void setup() {
  // setup Serial
  Serial.begin(115200);
  while(!Serial);

  // setup SPI
  Serial.println("setting up SPI");
  SPI.setMISO(PD13);
  SPI.setMOSI(PD12);
  SPI.setSCLK(PB2);
  SPI.begin();

  // setup flash
  Serial.println("setting up spi-flash");
  flash.begin(SPI, PB6);
  flash.erase();

  // write to flash
  Serial.println("writing data to spi-flash");
  flash.seek(0);
  for (int j = 0; j < 1000; j++) {
    flash.write((uint8_t*)&j, sizeof(int));
  }
}

void loop() {
  Serial.println("reading data from spi-flash:");
  // read from flash
  flash.seek(0);
  while (flash.available() > 0) {
    int result = 0;
    flash.readBytes((uint8_t*)&result, sizeof(int));
    Serial.println(result);
  }
}