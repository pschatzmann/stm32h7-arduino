# DevEBox STM32H7XX

There are two versions with different STM32H7 chips:

- STM32H750VBT6 - 128K flash
- STM32H743VIT6 - 2048K flash

I recommend to use the STM32H743VIT6 with 2MB of flash memory. 

![board](DevEBox.jpg)

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

## How to enter ISP mode (for Upload)

Enable the flash mode by connecting the SWD Boot0 pin with the adjecent 3V3 pin and then reboot. After you have flashed the device remove the connection and reboot again.


