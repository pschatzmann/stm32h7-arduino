
# STM32H7XX

There are different cheap but powerful development boards with different STM32H7 chips.
The STM32H7XX processors are based on the 32-bit Arm Cortex®-M7 core, running at up to 600 MHz. They have plenty of RAM and Flash Memory and have a FPU for fast double-precision floating calculations. In addition you also have plenty of pins (> 88) available!

I am not a big fan of the STM Cube IDE and find the approach of using generated code a bit "retro". Fortunately we can use __Arduino__. Because these boards are not well documented, I am taking the oppurtunity to demonstrate how to use them with Arduino.

### DevEBox

- STM32H750VBT6 - 128K flash
- STM32H743VIT6 - 2048K flash

![board](docs/DevEBox/STM32H7XX_M.jpg)

I recommend to use the STM32H743VIT6 with 2MB of flash memory. Please note that this is still only half of what you get with an ESP32! 

- [Further hardware details can be found here](docs/DevEBox/README.md)
- [The example sketches can be found here](examples/DevEBox)

### Software Requirements

- Install the [STMDuino Core](https://github.com/stm32duino)
- Install the [STMCubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html): This is needed to upload the compiled sketch


### Compile and Upload

#### Using USB Cable

You can upload your sketch w/o any additional programmer:

- Set the microcontroller into upload mode: e.g. by connecting the BTO with the 3.3V pin
- In Arduino
    - Select the Board "Generic STMH7 Series"
    - in -> Tools -> Board Part Number select your board: e.g. DevEBox...
    - in -> Tools -> Upload Method: Select DFU. This will use the USB cable
    - in -> Tools -> USB Support - CDC generic supercedes Usart
    - in -> Tools -> Usart Support - Enabled (generic serial)

With the help of these settings you will be able the to see the output of Serial.println() in the Arduino Serial Monitor.

You are ready now to compile and upload the sketch the regular way.
Remove the connection between BTO and 3.3V and reboot.


#### Using A Programmer

I recommend to use a [STLink Programmer](https://www.aliexpress.com/item/1005005273159580.html?spm=a2g0o.productlist.main.3.55421417bcZVae): This allows you debug your code and you do not need to bother with the upload mode, because this is handled automatically.

- Connect the GND, 3.3V, DIO and CLK pins from the board with the programmer
- In Arduino
    - Select the Board "Generic STMH7 Series"
    - in -> Tools -> Board Part Number select your board: e.g. DevEBox...
    - in -> Tools -> Upload Method: Select SWD
    - in -> Tools -> USB Support - CDC generic supercedes Usart
    - in -> Tools -> Usart Support - Enabled (generic serial)

You are ready now to compile and upload the sketch the regular way.
Please note that you still need the USB connection to see the Serial output!
