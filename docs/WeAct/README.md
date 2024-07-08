# WeAct STM32H7xx Core Board 
- `0.96''` ST7735 TFT, 
- TF Card, 
- 8MB SPI FLASH(SPI1)， 
- 8MB QSPI FLASH(QSPI1), 
- 8Bit DVP Port
- supports openmv firmware, see SDK/openmv

![alt text](Images/image-0.png)

### Interface and Keys
* 2*22 Pin 2.54mm I/O x 2
* 4 Pin 2.54mm SW x 1
* USB C (type C)  x 1
* MicroSD TF x 1
* 8Bit DCMI x 1
* User Key K1 (PC13) x 1
* NRST Key x 1
* BOOT0 Key x 1

## STM32H750VBT6 128KB ROM, 1MB RAM
* ARM Cortex M7
* 480Mhz Max. Freq
* 128KB ROM, 1MB RAM
* 8MB SPI Flash, 8MB QSPI Flash

## STM32H743VIT6 2048KB ROM, 1MB RAM
* ARM Cortex M7
* 480Mhz Max. Freq
* 2048KB ROM, 1MB RAM
* 8MB SPI Flash, 8MB QSPI Flash


### Interface and Keys
* 2*22 Pin 2.54mm I/O x 2
* 4 Pin 2.54mm SW x 1
* USB C (type C)  x 1
* MicroSD TF x 1
* 8Bit DCMI x 1
* User Key K1 (PC13) x 1
* NRST Key x 1
* BOOT0 Key x 1


### Power supply and Board Shape
* Input Voltage: 3.3V-5.5V
* DC-DC Output Current: 1A Max.
* Size: 40.64mm * 66.88mm

## How to enter ISP mode (for Upload)

* Method 1: When the power is on, press the BOOT0 key and the reset key, then release the reset key, and release the BOOT0 key after 0.5 seconds

* Method 2: When the power is off, hold down the BOOT0 key, and release the BOOT0 at 0.5s after the power is on


## Further Information

- [Hardware information](https://github.com/WeActStudio/MiniSTM32H7xx)
