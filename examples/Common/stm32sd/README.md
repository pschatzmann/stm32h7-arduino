# Using the stm32duino STM32SD library

Install the following libraries

- https://github.com/stm32duino/STM32SD
- https://github.com/stm32duino/FatFs


This is using SD card slots connected to the SDIO-/SDMMC-hardware of the processor. This is much faster then the alternative libraries which use SPI

The following Pins are used:

![board](SD-TF.png)

- PC10 D2
- PC11 D3
- PD2 CMD
- PC12 SCK
- PC8 D0
- PC9 D1

In order to set up the pins you need to do the following:

```C++
  // setup SD Pins for DevEBox MCU
  card.setDx(PC8, PC9, PC10, PC11);
  card.setCMD(PD2); // using PinName
  card.setCK(PC12);
```