# DevEBox Pins

#### User Button (K1)

- PE3 (K1)
- PC5 (K2)

The buttons are active low.

#### User LED (D2)

- PA1

#### Micro SD (U5)

- PC10 D2
- PC11 D3
- PD2 CMD
- PC12 SCK
- PC8 D0
- PC9 D1

```C++
  // setup SD Pins for DevEBox MCU
  card.setDx(PC8, PC9, PC10, PC11);
  card.setCMD(PD2); // using PinName
  card.setCK(PC12);
```

The Micro SD has no CS Pin, so we can not share the SPI port!
In Arduino we can use the SD API in 1 bit mode using SPI, so we need to define the relevant pins in SPI:

```C++
  SPI.setMISO(PC8);
  SPI.setMOSI(PD2);
  SPI.setSCLK(PC12);
```

#### QSPI Flash W25Q64 (U3)

- PB6 CS
- PD12 DO (IO01)
- PE2 WP (IO02)
- PD11 HOLD (IO03)
- PB2 CLK
- PD13 DI (O3)
- VCC 3V3

The HOLD pin must be set high and Write Protect (WP) set to low to enable writes.
In Arduino we can use the SPI API in 1 bit mode, so we need to define the relevant pins in SPI:

```C++
  SPI.setMISO(PD13);
  SPI.setMOSI(PD12);
  SPI.setSCLK(PB2);

  //The CS pin needs to be set to PB6
  Flash.begin(SPI, PB6);

```

#### Exposed Port Pins

- PA0-PA15
- PB0-PB15
- PC0-PC13 (PC14 OSC32_IN and PC15 OSC32_OUT not broken out)
- PD0-PD15
- PE0-PE15

Note: The PB15 next to PE14 is mislabelled. It should be PE15.

#### TFT/OLED (J4)

- 3V3
- GND
- PB15 MOSI
- PB13 SCK
- PB12 CS
- PB14 MISO
- PB1 RS (Reset Pin)
- PB0 BLK (Backgound Light)


#### SWD debug (J1)

- Boot0
- 3V3
- GND
- PA13 SWDIO
- PA14 SWCLK

Connect the Boot0 to the 3V3 pin to enable Flash Mode.

#### USB (J5)

- VCC 5V
- PA11 USB_DM
- PA12 USB_DP
- NC ID
- GND




