# DevEBox STM32H7XX

There are two versions with different STM32H7 chips:

- STM32H750VBT6 - 128K flash
- STM32H743VIT6 - 2048K flash

I recommend to use the STM32H743VIT6 with 2MB of flash memory. Please note that this is still only half of what you get with an ESP32! 

![board](STM32H7XX_M.jpg)

## Specifications

### STM32H750VBT6 

- STM32H750VBT6 ARM Cortex M7
- 128 Kbyte Flash, 1 Mbyte RAM

### STM32H743VIT6 

- STM32H743VIT6 ARM Cortex M7
- 2 Mbyte Flash, 1 Mbyte RAM

### Common 

- 400MHz, 1027 DMIPS / 2.14 DMIPS / MHz
- 25MHz system crystal
- 32.768KHz RTC crystal
- SWD header (BOOT0, 3V3, GND, PA13/SWDIO, PA14/SWCLK)
- 6x SPI, 4x USART, 4x UART, 1x LPUART, 3x I2S, 1x LPI2S, 4x I2C
- 4x SAI, SPDIFRX, SWPMI, MDIO Slave, 2x SD/SDIO/MMC, 2x CAN
- 2x USB OTG, 1x FS, 1x HS/FS
- Micro SD
- Winbond W25Q64 64Mbit QSPI Flash
- 1x 10/100 Ethernet MAC with DMA
- HDMI-CEC
- 1x 8 to 14-bit Parallel Camera interface
- 3x ADC (16-bit / 36-channel)
- 1x temperature sensor
- 2x DAC (12-bit)
- 2x ULP comparators
- 2x Op Amps
- 1x high res timer, 2x 32-bit timers, 2x advanced timers, 10x general timers,
- 5x low power timers, 2x watchdogs, 1x systick timer
- 96-bit unique ID
- AMS1117-3.3V: 3.3V LDO voltage regulator, max current 800mA
- Micro USB for power and comms
- Power LED D1
- User LED D2 (PA1) active low
- Reset button, 2x user buttons K1 (PE3), K2 (PC5)
- 2x22 2.54mm side headers
- DCMI camera interface (OV2640, OV5640)
- SPI TFT/OLED header (3V3, GND, SDI, SCL, CS, SDO, D/C, BLK)
- RTC battery header B1 beside camera interface
- M3 mounting holes
- Dimensions: 40.89mm x 68.59mm

## Pins

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

#### Micro SD (U5)

- PC10 D2
- PC11 D3
- PD2 CMD
- PC12 SCK
- PC8 D0
- PC9 D1

The Micro SD has no CS Pin, so we can not share the SPI port!
In Arduino we can use the SD API in 1 bit mode, so we need to define the relevant pins in SPI:

```C++
  SPI.setMISO(PC8);
  SPI.setMOSI(PD2);
  SPI.setSCLK(PC12);
```


#### User Button (K1)

- PE3 (K1)
- PC5 (K2)


#### User LED (D2)

- PA1


## Flashing Device

#### Flashing device via USB cable

1. Enable the flash mode by bridging the Boot0 pin with the adjecent 3V3 pin
2. Reboot
3. In Arduino
    - Select Upload Method SWD
    - Compile and upload your sketch
5. Remove Boot0 - 3V3 bridge
6. Reboot


## Futher Information

- [STM32H750VB on st.com](https://www.st.com/en/microcontrollers-microprocessors/stm32h750vb.html)
- [STM32H743VI on st.com](https://www.st.com/en/microcontrollers-microprocessors/stm32h743vi.html)
- [STM32H7XX_M schematics](https://github.com/mcauser/MCUDEV_DEVEBOX_H7XX_M/blob/master/docs/STM32H7XX_M_schematics.pdf)
- [External Loader](https://controllerstech.com/w25q-flash-series-part-10-qspi-ext-loader-in-h750/)

## License

Licensed under the [MIT License](http://opensource.org/licenses/MIT).
