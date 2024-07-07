# Using the SDFAT Library

This library uses SPI to communicated with the SD card.
The following SPI Pins are used:

![board](SD-TF.png)

- PD2 CMD
- PC12 SCK
- PC8 D0

In order to set up the pins you need to do the following:

```C++
  // Define SPI pins for the SD card
  SPI.setMISO(PC8);
  SPI.setMOSI(PD2);
  SPI.setSCLK(PC12);
```