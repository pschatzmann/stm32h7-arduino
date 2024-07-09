/***
 * The w25qxx flash memory can not be used via SPI, so we must be using the STM32 QSPI API.
 * I have included some small library that can be used to write and read the data.
 * @author Phil Schatzmann
 */

#include "QSPI.h"

const char* msg = "this is a test";

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("begin");
  QSPI.begin();

  Serial.println("erase...");
  if (!QSPI.eraseChip()) {
    Serial.println("Error: CSP_QSPI_Erase_Chip");
  }

  Serial.println("writing...");
  if (QSPI.write((const uint8_t*)msg, strlen(msg)+1) == 0) {
    Serial.println("Error: CSP_QSPI_WriteMemory");
  };

  uint8_t buffer[120];
  QSPI.seek(0);
  QSPI.readBytes(buffer, 120);
  Serial.println((char*)buffer);

  Serial.println("Done");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}
