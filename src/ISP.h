

#define BOOT_ADDR 0x1FF0A000 // 0x1FF09800 ? // my MCU boot code base address

/**
 * Set Processor to ISP mode
 * 
 */

class ISP {
 public:
  void jumpToBootloader(void) {
    /* Disable all interrupts */
    __disable_irq();

    /* Disable Systick timer */
    SysTick->CTRL = 0;

    /* Set the clock to the default state */
    HAL_RCC_DeInit();

    /* Clear Interrupt Enable Register & Interrupt Pending Register */
    for (int i = 0; i < sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]); i++) {
      NVIC->ICER[i] = 0xFFFFFFFF;
      NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    /* Re-enable all interrupts */
    __enable_irq();

    // Set the stack pointer
    __set_MSP((uint32_t)BOOT_ADDR);

    // Jump to app firmware
    void (*Reset_Handler)(void) = (void (*)(void)) (BOOT_ADDR + 4);
    Reset_Handler();
  }
};