#include <SPI.h>
#include <SerialFlash.h> // https://github.com/PaulStoffregen/SerialFlash

const int FlashChipSelect = PB6; // digital pin for flash chip CS pin

void setup() {
  // DevEBox STM32H7XX pins
  SPI.setMISO(PD13);
  SPI.setMOSI(PD12);
  SPI.setSCK(PB2);


  Serial.begin(44100);

  // wait for Arduino Serial Monitor
  while (!Serial) ;
  delay(100);
  Serial.println(F("All Files on SPI Flash chip:"));

  if (!SerialFlash.begin(FlashChipSelect)) {
    while (1) {
      Serial.println(F("Unable to access SPI Flash chip"));
      delay(2500);
    }
  }

  SerialFlash.opendir();
  int filecount = 0;
  while (1) {
    char filename[64];
    uint32_t filesize;

    if (SerialFlash.readdir(filename, sizeof(filename), filesize)) {
      Serial.print(F("  "));
      Serial.print(filename);
      Serial.print(F(", "));
      Serial.print(filesize);
      Serial.print(F(" bytes"));
      SerialFlashFile file = SerialFlash.open(filename);
      if (file) {
        unsigned long usbegin = micros();
        unsigned long n = filesize;
        char buffer[256];
        while (n > 0) {
          unsigned long rd = n;
          if (rd > sizeof(buffer)) rd = sizeof(buffer);
          file.read(buffer, rd);
          n = n - rd;
        }
        unsigned long usend = micros();
        Serial.print(F(", read in "));
        Serial.print(usend - usbegin);
        Serial.print(F(" us, speed = "));
        Serial.print((float)filesize * 1000.0 / (float)(usend - usbegin));
        Serial.println(F(" kbytes/sec"));
        file.close();
      } else {
        Serial.println(F(" error reading this file!"));
      }
      filecount = filecount + 1;
    } else {
      if (filecount == 0) {
        Serial.println(F("No files found in SerialFlash memory."));
      }
      break; // no more files
    }
  }
}

void loop() {
}