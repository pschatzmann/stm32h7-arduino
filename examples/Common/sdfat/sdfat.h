/***
 * Reading data from the SD drive using the SdFat library.
 * Unfortunately the native STM32 SPI can not be used because
 * the required pins do not support SPI.
 * But we can use Software SPI!
 * 
 * You need to change SdFatConfig.h in the SdFatConfig.h
 *  @author Phil Schatzmann
 */
#include "SdFat.h" // https://github.com/greiman/SdFat

SdFs sd;
FsFile file;
SoftSpiDriver<PC8, PD2, PC12> softSpi;
SdSpiConfig cfg{PC11, DEDICATED_SPI, SD_SCK_MHZ(21), &softSpi};

void setup() {
  Serial.begin(9600);
  while(!Serial);

  Serial.println("SD begin");
  if (!sd.begin(cfg)) {
    Serial.println("sd error");
    while (true);
  }
  Serial.println("writing data...");
  uint64_t start = millis();
  file.open("/test.txt", O_RDWR | O_CREAT | O_TRUNC);
  for (int j = 0; j < 100; j++) {
    file.println(j);
  }
  file.close();

  file.open("/test.txt", O_RDONLY );
  while (file.available()>0) {
    Serial.println(file.readString());
  }

  Serial.print("time: ");
  Serial.println(millis() - start);

}

void loop() {
  delay(1000);
}