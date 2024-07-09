#pragma once
#include "Stream.h"
#include "stm32h7xx_hal.h"

/*W25Q32 memory parameters*/
#define MEMORY_FLASH_SIZE 0x800000 /* 64Mbit =>8Mbyte */
#define MEMORY_BLOCK_SIZE 0x10000  /*  blocks of 64KBytes */
#define MEMORY_SECTOR_SIZE 0x1000  /* 4kBytes */
#define MEMORY_PAGE_SIZE 0x100     /* 256 bytes */
#define MEMORY_current_pos 0x90000000

/*W25Q64JV commands */
#define CHIP_ERASE_CMD 0xC7
#define READ_STATUS_REG_CMD 0x05
#define WRITE_ENABLE_CMD 0x06
#define VOLATILE_SR_WRITE_ENABLE 0x50
#define READ_STATUS_REG2_CMD 0x35
#define WRITE_STATUS_REG2_CMD 0x31
#define READ_STATUS_REG3_CMD 0x15
#define WRITE_STATUS_REG3_CMD 0x11
#define SECTOR_ERASE_CMD 0x20
#define BLOCK_ERASE_CMD 0xD8
#define QUAD_IN_FAST_PROG_CMD 0x32
#define FAST_PROG_CMD 0x02
#define QUAD_OUT_FAST_READ_CMD 0x6B
#define DUMMY_CLOCK_CYCLES_READ_QUAD 8
#define QUAD_IN_OUT_FAST_READ_CMD 0xEB
#define RESET_ENABLE_CMD 0x66
#define RESET_EXECUTE_CMD 0x99

/***
 * C++ API for w25qxx flash memory which uses the STM32 API. The Arduino SPI API
 * is used to communicate with the memory.
 * @author Phil Schatzmann
 */

class QSPI : public Stream {
 public:
  /// Changes the actual position
  bool seek(size_t pos) {
    if (pos >= MEMORY_FLASH_SIZE) return false;
    current_pos = pos;
    return true;
  }

  /// Provides the actual position
  uint64_t position() { return current_pos; }

  /// QUADSPI init function
  bool begin(void) {
    MPU_Config();
    hqspi.Instance = QUADSPI;
    init();

    if (HAL_QSPI_DeInit(&hqspi) != HAL_OK) {
      Serial.println("HAL_QSPI_DeInit");
      return false;
    }

    // if (!reset()) {
    //   Serial.println("reset");
    //   //return false;
    // }

    HAL_Delay(1);

    if (!wait()) {
      Serial.println("wait");
      return false;
    }

    if (!setWriteEnabled()) {
      Serial.println("setWriteEnabled");
      return false;
    }

    if (!setQuadMode()) {
      Serial.println("setQuadMode");
      return false;
    }

    HAL_Delay(100);
    current_pos = 0;

    return true;
  }

  /// Stopping QuadSPI
  void end() {
    if (hqspi.Instance == QUADSPI) {
      /* Peripheral clock disable */
      __HAL_RCC_QSPI_CLK_DISABLE();

      /**QUADSPI GPIO Configuration
      PE2     ------> QUADSPI_BK1_IO2
      PB2     ------> QUADSPI_CLK
      PD11     ------> QUADSPI_BK1_IO0
      PD12     ------> QUADSPI_BK1_IO1
      PD13     ------> QUADSPI_BK1_IO3
      PB6     ------> QUADSPI_BK1_NCS
      */
      HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);

      HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2 | GPIO_PIN_6);

      HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);
    }
  }

  /// erase the chip
  bool eraseChip(void) {
    QSPI_CommandTypeDef sCommand = {0};

    if (!setWriteEnabled()) {
      return false;
    }

    /* Erasing Sequence --------------------------------- */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = CHIP_ERASE_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_MAX_DELAY) != HAL_OK) {
      return false;
    }

    if (!wait()) {
      return false;
    }

    return true;
  }

  /// erase a single block
  bool eraseBlock(uint32_t block) {
    QSPI_CommandTypeDef sCommand = {0};
    HAL_StatusTypeDef ret;

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations ------------------------------------------- */
    if ((!setWriteEnabled())) {
      return false;
    }

    /* Erasing Sequence -------------------------------------------------- */
    sCommand.Instruction = BLOCK_ERASE_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
    sCommand.Address = block * blockSize();  // determine address
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;

    if ((ret = HAL_QSPI_Command(&hqspi, &sCommand,
                                HAL_QPSI_TIMEOUT_DEFAULT_VALUE)) != HAL_OK) {
      return false;
    }
    return true;
  }

  /// erase a range of sectors
  bool eraseSector(uint32_t startSector, uint32_t endSector) {
    QSPI_CommandTypeDef sCommand;

    // startSector = startSector - startSector % MEMORY_SECTOR_SIZE;

    /* Erasing Sequence -------------------------------------------------- */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = SECTOR_ERASE_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;

    if (!setWriteEnabled()) {
      return false;
    }

    while (endSector >= startSector) {
      sCommand.Address = startSector * sectorSize();

      if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
          HAL_OK) {
        return false;
      }
      startSector++;

      if (!wait()) {
        return false;
      }
    }

    return true;
  }

  size_t write(const uint8_t* buffer, size_t buffer_size) {
    QSPI_CommandTypeDef sCommand;
    uint32_t end_addr, current_size, current_addr;
    size_t result = 0;

    /* Calculation of the size between the write current_pos and the end of the
     * page
     */
    current_addr = 0;

    //
    while (current_addr <= current_pos) {
      current_addr += MEMORY_PAGE_SIZE;
    }
    current_size = current_addr - current_pos;

    /* Check if the size of the data is less than the remaining place in the
     * page */
    if (current_size > buffer_size) {
      current_size = buffer_size;
    }

    /* Initialize the address variables */
    current_addr = current_pos;
    end_addr = current_pos + buffer_size;

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.DataMode = QSPI_DATA_4_LINES;
    sCommand.NbData = buffer_size;
    sCommand.Address = current_pos;
    sCommand.DummyCycles = 0;

    /* Perform the write page by page */
    do {
      sCommand.Address = current_addr;
      sCommand.NbData = current_size;

      if (current_size == 0) {
        current_pos += result;
        if (current_pos > max_current_pos) max_current_pos = current_pos;
        return result;
      }

      /* Enable write operations */
      if (!setWriteEnabled()) {
        current_pos += result;
        if (current_pos > max_current_pos) max_current_pos = current_pos;
        return result;
      }

      /* Configure the command */
      if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
          HAL_OK) {
        current_pos += result;
        if (current_pos > max_current_pos) max_current_pos = current_pos;
        return result;
      }

      /* Transmission of the data */
      if (HAL_QSPI_Transmit(&hqspi, (uint8_t*)buffer,
                            HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        current_pos += result;
        if (current_pos > max_current_pos) max_current_pos = current_pos;
        return result;
      }

      /* Configure automatic polling mode to wait for end of program */
      if (!wait()) {
        current_pos += result;
        if (current_pos > max_current_pos) max_current_pos = current_pos;
        return result;
      }

      /* Update the current_pos and size variables for next page programming */
      current_addr += current_size;
      buffer += current_size;
      current_size = ((current_addr + MEMORY_PAGE_SIZE) > end_addr)
                         ? (end_addr - current_addr)
                         : MEMORY_PAGE_SIZE;
      result += current_size;
    } while (current_addr <= end_addr);

    // increase current current_pos
    current_pos += result;
    if (current_pos > max_current_pos) max_current_pos = current_pos;

    return result;
  }

  bool setMemoryMappedModeEnabled(void) {
    QSPI_CommandTypeDef sCommand;
    QSPI_MemoryMappedTypeDef sMemMappedCfg;

    /* Enable Memory-Mapped
     * mode-------------------------------------------------- */

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = QUAD_IN_OUT_FAST_READ_CMD;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
    sCommand.Address = 0;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES;
    sCommand.AlternateBytes = 0xFF;
    sCommand.AlternateBytesSize = 1;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.DataMode = QSPI_DATA_4_LINES;
    sCommand.NbData = 0;
    sCommand.DummyCycles = 4;

    sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    sMemMappedCfg.TimeOutPeriod = 0;

    if (HAL_QSPI_MemoryMapped(&hqspi, &sCommand, &sMemMappedCfg) != HAL_OK) {
      return false;
    }
    HAL_Delay(100);
    is_memory_mapped = true;
    return true;
  }

  size_t readBytes(uint8_t* pData, uint32_t bytes) {
    size_t result = 0;
    size_t bytes_to_read = bytes;
    if (current_pos + bytes > max_current_pos) {
      bytes_to_read = max_current_pos - current_pos;
    }
    QSPI_CommandTypeDef sCommand;
    uint32_t ReadAddr = current_pos;
    /* Initialize the read command */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = QUAD_IN_OUT_FAST_READ_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.Address = ReadAddr;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_4_LINES;
    sCommand.DummyCycles = 6U;
    sCommand.NbData = bytes_to_read;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
        HAL_OK) {
      return result;
    }

    /* Set S# timing for Read command */
    MODIFY_REG(hqspi.Instance->DCR, QUADSPI_DCR_CSHT,
               QSPI_CS_HIGH_TIME_5_CYCLE);

    /* Reception of the data */
    if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
        HAL_OK) {
      return result;
    }
    result = bytes_to_read;

    /* Restore S# timing for nonRead commands */
    MODIFY_REG(hqspi.Instance->DCR, QUADSPI_DCR_CSHT,
               QSPI_CS_HIGH_TIME_6_CYCLE);

    return result;
  }

  /// Avoid the slow single byte operations
  size_t write(uint8_t ch) override { return write(&ch, 1); }

  int available() { return max_current_pos; }

  int availableForWrite() { return size() - current_pos; }

  size_t size() { return MEMORY_FLASH_SIZE; }

  size_t blockCount() { return MEMORY_FLASH_SIZE / MEMORY_BLOCK_SIZE; }

  size_t blockSize() { return MEMORY_BLOCK_SIZE; }

  size_t sectorCount() { return MEMORY_FLASH_SIZE / MEMORY_SECTOR_SIZE; }

  size_t sectorSize() { return MEMORY_SECTOR_SIZE; }

  /// Avoid the slow single byte operations
  int read() override {
    uint8_t data[1];
    return readBytes(data, 1);
  }

  /// Avoid the slow single byte operations
  int peek() override {
    int result = read();
    if (result > 0) current_pos -= result;
    return result;
  }

  // Returns the current mapped address if available otherwise nullptr
  void* address(size_t basecurrent_pos = MEMORY_current_pos) {
    if (!is_memory_mapped || current_pos > MEMORY_FLASH_SIZE) return nullptr;
    uint8_t* ptr = (uint8_t*)basecurrent_pos;
    return ptr + position();
  }

  /// the maximum written bytes are not persisted: you need to provide this
  /// yourself to the API
  void setAvailable(int max) { max_current_pos = max; }

 protected:
  QSPI_HandleTypeDef hqspi;
  uint64_t current_pos = 0;
  size_t max_current_pos = 0;
  bool is_memory_mapped = false;

  bool wait(void) {
    QSPI_CommandTypeDef sCommand = {0};
    QSPI_AutoPollingTypeDef sConfig = {0};
    HAL_StatusTypeDef ret;

    /* Configure automatic polling mode to wait for memory ready ------ */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = READ_STATUS_REG_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_1_LINE;
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.Match = 0x00;
    sConfig.Mask = 0x01;
    sConfig.MatchMode = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval = 0x10;
    sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;
    if ((ret = HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig,
                                    HAL_MAX_DELAY)) != HAL_OK) {
      return false;
    }
    return true;
  }

  /* QUADSPI init function */
  void init(void) {
    hqspi.Instance = QUADSPI;
    hqspi.Init.ClockPrescaler = 1;
    hqspi.Init.FifoThreshold = 4;
    hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    hqspi.Init.FlashSize = 22;
    hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
    hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
    hqspi.Init.FlashID = QSPI_FLASH_ID_1;
    hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
    if (HAL_QSPI_Init(&hqspi) != HAL_OK) {
      Serial.println("init");
      Error_Handler();
    }
  }

  void init(QSPI_HandleTypeDef* qspiHandle) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if (qspiHandle->Instance == QUADSPI) {
      /** Initializes the peripherals clock
       */
      PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_QSPI;
      PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;
      if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        Serial.println("HAL_RCCEx_PeriphCLKConfig");
        Error_Handler();
      }

      /* QUADSPI clock enable */
      __HAL_RCC_QSPI_CLK_ENABLE();

      __HAL_RCC_GPIOE_CLK_ENABLE();
      __HAL_RCC_GPIOB_CLK_ENABLE();
      __HAL_RCC_GPIOD_CLK_ENABLE();
      /**QUADSPI GPIO Configuration
      PE2     ------> QUADSPI_BK1_IO2
      PB2     ------> QUADSPI_CLK
      PD11     ------> QUADSPI_BK1_IO0
      PD12     ------> QUADSPI_BK1_IO1
      PD13     ------> QUADSPI_BK1_IO3
      PB6     ------> QUADSPI_BK1_NCS
      */
      GPIO_InitStruct.Pin = GPIO_PIN_2;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
      HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = GPIO_PIN_2;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
      HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = GPIO_PIN_6;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
  }

  bool reset() {
    QSPI_CommandTypeDef sCommand = {0};
    uint32_t temp = 0;
    HAL_StatusTypeDef ret;

    /* Enable Reset --------------------------- */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = RESET_ENABLE_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if ((ret = HAL_QSPI_Command(&hqspi, &sCommand,
                                HAL_QPSI_TIMEOUT_DEFAULT_VALUE)) != HAL_OK) {
      Serial.println("RESET_ENABLE_CMD");
      return false;
    }

    /* Reset Device --------------------------- */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = RESET_EXECUTE_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if ((ret = HAL_QSPI_Command(&hqspi, &sCommand,
                                HAL_QPSI_TIMEOUT_DEFAULT_VALUE)) != HAL_OK) {
      Serial.println("RESET_EXECUTE_CMD");
      return false;
    }

    for (temp = 0; temp < 500000; temp++) {
      __NOP();
    }

    return true;
  }

  bool setWriteEnabled(void) {
    QSPI_CommandTypeDef sCommand = {0};
    QSPI_AutoPollingTypeDef sConfig = {0};
    HAL_StatusTypeDef ret;

    /* Enable write operations ------------------------------------------ */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = WRITE_ENABLE_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if ((ret = HAL_QSPI_Command(&hqspi, &sCommand,
                                HAL_QPSI_TIMEOUT_DEFAULT_VALUE)) != HAL_OK) {
      Serial.println("WRITE_ENABLE_CMD");
      return false;
    }

    /* Configure automatic polling mode to wait for write enabling ---- */
    sConfig.Match = 0x02;
    sConfig.Mask = 0x02;
    sConfig.MatchMode = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval = 0x10;
    sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    sCommand.Instruction = READ_STATUS_REG_CMD;
    sCommand.DataMode = QSPI_DATA_1_LINE;

    if ((ret = HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig,
                                    HAL_QPSI_TIMEOUT_DEFAULT_VALUE)) !=
        HAL_OK) {
      Serial.println("HAL_QSPI_AutoPolling");
      return false;
    }
    return true;
  }

  /*Enable quad mode*/
  /* The versions ending in IQ have it activated by default, as in my
   * case
. */

  bool setQuadMode(void) {
    QSPI_CommandTypeDef sCommand = {0};
    uint8_t reg;
    HAL_StatusTypeDef ret;

    /* Read Volatile Configuration register 2 --------------------------- */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = READ_STATUS_REG2_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_1_LINE;
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.NbData = 1;

    if ((ret = HAL_QSPI_Command(&hqspi, &sCommand,
                                HAL_QPSI_TIMEOUT_DEFAULT_VALUE)) != HAL_OK) {
      Serial.println("READ_STATUS_REG2_CMD");
      return false;
    }

    if ((ret = HAL_QSPI_Receive(&hqspi, &reg,
                                HAL_QPSI_TIMEOUT_DEFAULT_VALUE)) != HAL_OK) {
      Serial.println("HAL_QSPI_Receive");
      return false;
    }

    /* Enable Volatile Write operations ----------------------------------------
     */
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.Instruction = VOLATILE_SR_WRITE_ENABLE;

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
        HAL_OK) {
      Serial.println("VOLATILE_SR_WRITE_ENABLE");
      return false;
    }

    /* Write Volatile Configuration register 2 (QE = 1) -- */
    sCommand.DataMode = QSPI_DATA_1_LINE;
    sCommand.Instruction = WRITE_STATUS_REG2_CMD;
    reg |= 2;  // QE bit

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
        HAL_OK) {
      Serial.println("HAL_QPSI_TIMEOUT_DEFAULT_VALUE");
      return false;
    }

    if (HAL_QSPI_Transmit(&hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
        HAL_OK) {
      Serial.println("HAL_QSPI_Transmit");
      return false;
    }

    /* Read Volatile Configuration register 3 --------------------------- */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = READ_STATUS_REG3_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_1_LINE;
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.NbData = 1;

    if ((ret = HAL_QSPI_Command(&hqspi, &sCommand,
                                HAL_QPSI_TIMEOUT_DEFAULT_VALUE)) != HAL_OK) {
      Serial.println("READ_STATUS_REG3_CMD");
      return false;
    }

    if ((ret = HAL_QSPI_Receive(&hqspi, &reg,
                                HAL_QPSI_TIMEOUT_DEFAULT_VALUE)) != HAL_OK) {
      Serial.println("HAL_QSPI_Receive");
      return false;
    }

    /* Write Volatile Configuration register 2 (DRV1:2 = 00) -- */
    sCommand.Instruction = WRITE_STATUS_REG3_CMD;
    reg &= 0x9f;  // DRV1:2 bit

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
        HAL_OK) {
      Serial.println("WRITE_STATUS_REG3_CMD");
      return false;
    }

    if (HAL_QSPI_Transmit(&hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
        HAL_OK) {
      Serial.println("HAL_QSPI_Transmit");
      return false;
    }

    return true;
  }

  void MPU_Config(void) {
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    /* Disables the MPU */
    HAL_MPU_Disable();

    /** Initializes and configures the Region and the memory to be protected
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = 0x90000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;
    MPU_InitStruct.SubRegionDisable = 0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    /* Enables the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
  }
};
