# WeAct Pins

![alt text](Images/image-1.png)

## Buttons & LED

#### Buttons 

- NR - NRST
- Boot - BOOT0
- K1: PC13 


#### User LED 

- PE3 - Blue LED


## Peripherals

![alt text](Images/image.png)

#### Micro SD (U5)

- PC10 D2  - 
- PC11 D3  CS
- PD2  CMD DI(MOSI)
- PC12 SCK SCLK
- PC8  D0  DO (MISO)
- PC9  D1  -

```C++
  // setup SD Pins for DevEBox MCU
  card.setDx(PC8, PC9, PC10, PC11);
  card.setCMD(PD2); // using PinName
  card.setCK(PC12);
```

Unfortunatly we can not directly use these pins to access the drive via SPI. The only work around would be to use the proper SPI pins and connnect the pins above pysically to to use a bitbanging SPI library.

#### QSPI Flash W25Q64 (U3)

- PB6 CS
- PD12 DO (IO01)
- PE2 WP (IO02)
- PD11 HOLD (IO03)
- PB2 CLK
- PD13 DI (O3)
- VCC 3V3

The HOLD pin must be set high and Write Protect (WP) set to low to enable writes.
