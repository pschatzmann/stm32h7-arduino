#pragma once
#include "Stream.h"
#include "W25Q64/quadspi.h"

/***
 * C++ API for w25qxx flash memory which uses the STM32 API communicate with the
 * memory.
 * @author Phil Schatzmann
 */

class QSPIClass : public Stream {
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
    MX_QUADSPI_Init();

    if (CSP_QUADSPI_Init() != HAL_OK) {
      Serial.println("CSP_QUADSPI_Init");
      return false;
    };

    return true;
  }

  /// Stopping QuadSPI
  void end() { MX_QUADSPI_DeInit(); }

  /// erase the chip
  bool eraseChip(void) {
    current_pos = 0;
    max_current_pos = 0;
    return CSP_QSPI_Erase_Chip() == HAL_OK;
  }

  /// erase a single block
  bool eraseBlock(uint32_t flashAddress) {
    current_pos = flashAddress;
    max_current_pos = flashAddress;
    return CSP_QSPI_EraseBlock(flashAddress) == HAL_OK;
  }

  bool eraseSector(uint32_t from, uint32_t to) {
    current_pos = from;
    max_current_pos = from;
    return CSP_QSPI_EraseSector(from, to) == HAL_OK;
  }

  size_t write(const uint8_t* buffer, size_t buffer_size) {
    int rc = CSP_QSPI_WriteMemory((uint8_t*)buffer, current_pos, buffer_size);
    if (rc == HAL_OK) {
      current_pos += buffer_size;
      if (max_current_pos < current_pos) max_current_pos = current_pos;
    }
    return rc == HAL_OK ? buffer_size : 0;
  }

  bool setMemoryMappedModeEnabled(void) {
    return CSP_QSPI_EnableMemoryMappedMode() == HAL_OK;
  }

  size_t readBytes(uint8_t* buffer, uint32_t buffer_size) {
    int rc = CSP_QSPI_Read(buffer, current_pos, buffer_size);
    if (rc == HAL_OK) current_pos += buffer_size;
    return rc == HAL_OK ? buffer_size : 0;
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
  void* mappedAddress(size_t basecurrent_pos = MEMORY_FLASH_ADDRESS) {
    if (!is_memory_mapped || current_pos > MEMORY_FLASH_SIZE) return nullptr;
    uint8_t* ptr = (uint8_t*)basecurrent_pos;
    return ptr + position();
  }

  /// the maximum written bytes are not persisted: you need to provide this
  /// yourself to the API
  void setAvailable(int max) { max_current_pos = max; }

  /// wait for write operation to terminate
  void flush() { QSPI_AutoPollingMemReady(); }

  /// Bootloader functionality: jump to the indicated address
  void jumpToAddress(uint32_t address = MEMORY_FLASH_ADDRESS) {
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
    __set_MSP(address);

    // Jump to app firmware
    void (*Reset_Handler)(void) = (void (*)(void))(address + 4);
    Reset_Handler();
  }

 protected:
  uint64_t current_pos = 0;
  size_t max_current_pos = 0;
  bool is_memory_mapped = false;
};

static QSPIClass QSPI;