
#pragma once
#include "SPI.h"
#include "Stream.h"

typedef int pin_t;
typedef void (*FunctionPointer_t)(void);

/***
 * C++ API for w25qxx flash memory. The Arduino SPI API is used to communicate
 * with the memory.
 * @author Phil Schatzmann
 */

class FlashW25QXX : public Stream {
 public:
  // start the flash memory: make sure that you call begin() on the SPI object
  bool begin(SPIClass &spi = SPI, pin_t cs = -1) {
    bool status = true;
    uint8_t wr[1] = {0x00};
    p_spi = &spi;
    flash_cs = cs;

    // put your setup code here, to run once:
    if (flash_cs != -1) pinMode(flash_cs, OUTPUT);
    p_spi->beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

    // write_enable();
    setCSAcive(false);
    p_spi->transfer(0XAB);  // release POWER down

    if (!reset()) {
      return false;
    }

    //deviceID();
 
    // remove PROTECT
    status = write_register(0x01, wr, 1) == 0;
    // wait();
    
    address = 0;
    readBytes((uint8_t *)&max_address, sizeof(max_address));

    return status;
  }

  void end() {
    // write max used size
    address = 0;
    write((const uint8_t *)&max_address, sizeof(max_address));
    wait();

    // write_enable();
    setCSAcive(false);
    p_spi->transfer(0XB9);  // POWER down
  }

  bool seek(size_t pos) {
    if (pos >= PAGE_COUNT * PAGE_SIZE) return false;
    address = pos + sizeof(size_t);
    return true;
  }

  uint64_t position() { return address; }

  void wait() {
    uint8_t reg1[1], is_busy = 1;
    while (is_busy) {
      read_register(0x05, reg1, 1);
      is_busy = ((reg1[0]) & 0x01);
      delay(1);
    }
  }

  /// Erase the chip
  bool erase() {
    uint8_t status = 1;
    uint8_t reg1[1];
    // waiting
    wait();
    // enable
    set_write_enable(true);
    // erase
    status = write_cmd(0xC7);
    seek(0);
    return status == FLASH_OK;
  }

  /// Erase an individual Sector
  bool erase(uint16_t sector) {
    if (sector > SECTOR_SIZE) {
      return false;
    }
    // waiting for ready
    wait();
    // enable
    set_write_enable(true);
    // resume
    write_cmd(0x7A);
    // erase sector
    if (flash_cs != -1) digitalWrite(flash_cs, LOW);
    write_addr(0x20, sector * SECTOR_BYTE);
    if (flash_cs != -1) digitalWrite(flash_cs, HIGH);
    return true;
  }

  virtual size_t write(const uint8_t *data, size_t size) override {
    uint8_t status = FLASH_OK;
    int32_t page_byte = size;

    if (address + size > (PAGE_COUNT * PAGE_SIZE)) {
      return 0;
    }

    // waiting finish
    wait();

    // write map
    while (page_byte > 0) {
      // enable
      set_write_enable(true);

      size = ((page_byte > PAGE_SIZE) ? PAGE_SIZE : page_byte);
      // write page
      setCSAcive(true);
      write_addr(0x02, address);
      // write data

      for (uint8_t i = 0; i < (size); i++) {
        status = p_spi->transfer(data[i]);
      }
      setCSAcive(false);

      address += PAGE_SIZE;
      page_byte -= PAGE_SIZE;
    }

    address += size;
    if (max_address < address) max_address = address;
    return size;
  }

  virtual size_t readBytes(uint8_t *data, size_t size) {
    uint8_t status = FLASH_OK;
    setCSAcive(true);
    write_addr(0x03, address);
    for (uint32_t i = 0; i < size; i++) {
      data[i] = p_spi->transfer(0x00);
    }
    setCSAcive(false);
    address += size;
    return size;
  }

  /// Avoid the slow single byte operations
  size_t write(uint8_t ch) override { return write(&ch, 1); }

  int available() { return max_address; }

  int availableForWrite() { return (PAGE_COUNT * PAGE_SIZE) - address; }

  size_t size() { return PAGE_COUNT * PAGE_SIZE; }

  size_t pages() { return PAGE_COUNT; }

  size_t pageSize() { return PAGE_SIZE; }

  /// Avoid the slow single byte operations
  int read() override {
    uint8_t data[1];
    return readBytes(data, 1);
  }

  /// Avoid the slow single byte operations
  int peek() override {
    int result = read();
    if (result > 0) address -= result;
    return result;
  }

  uint16_t deviceID() {
    uint8_t read_data[3];
    read_register(0x9F, read_data, 3);
    return read_data[0] << 8 | read_data[1];
  }

  // enable Quad SPI
  bool setQuadMode() {
    uint8_t status = FLASH_OK;
    wait();
    status = write_cmd(0x38);
    return status == FLASH_OK;
  }

  /// the maximum written bytes are not persisted: you need to provide this
  /// yourself to the API
  void setAvailable(int max) { max_address = max; }

 protected:
  enum FlashStatus { FLASH_OK = 0, FLASH_ERR = 1, FLASH_ERR_PARAM = 2 };
  pin_t flash_cs = -1;
  // w25q32 defination
  uint32_t FLASH_BYTE = ((4) * 1024 * 1024);  // 4MB
  uint32_t SECTOR_BYTE = ((4) * 1024);        // 4kb
  uint32_t BLOCK_BYTE = ((64) * 1024);        // 64kb
  uint32_t PAGE_SIZE = ((1) * 256);           // 256byte
  // 4MB
  uint32_t PAGE_COUNT = (FLASH_BYTE / PAGE_SIZE);
  uint32_t SECTOR_SIZE = (FLASH_BYTE / SECTOR_BYTE);
  uint32_t BLOCK_SIZE = (FLASH_BYTE / BLOCK_BYTE);
  SPIClass *p_spi = nullptr;
  size_t address = 0;
  size_t max_address = 0;

  void read_register(uint8_t cmd, uint8_t *regdata, uint8_t size) {
    setCSAcive(true);
    p_spi->transfer(cmd);  // JEDEC ID komutunu gönder
    for (uint8_t i = 0; i < size; i++) {
      regdata[i] = p_spi->transfer(0x00);
    }
    setCSAcive(false);
  }

  FlashStatus write_register(uint8_t cmd, uint8_t *regdata, uint8_t size) {
    FlashStatus status = FLASH_OK;
    wait();
    setCSAcive(true);
    p_spi->transfer(cmd);  // WRITE ID komutunu gönder
    for (uint8_t i = 0; i < size; i++) {
      status = (FlashStatus)p_spi->transfer(regdata[i]);
    }
    setCSAcive(false);
    return status;
  }

  bool write_cmd(uint8_t cmd) {
    uint8_t status = FLASH_OK;
    setCSAcive(true);
    status = p_spi->transfer(cmd);  // RESUME komutunu gönder
    setCSAcive(false);
    return status == FLASH_OK;
  }

  void write_addr(uint32_t cmd, uint32_t address) {
    p_spi->transfer(cmd);
    p_spi->transfer((address >> 16) & 0xff);  //(MSB)
    p_spi->transfer((address >> 8) & 0xff);
    p_spi->transfer(address & 0xff);  // (LSB)
  }

  inline void setCSAcive(bool active) {
    if (flash_cs != -1) digitalWrite(flash_cs, active ? LOW : HIGH);
  }

  bool setWriteEnabled() {
    // Wait
    wait();

    write_cmd(0x06);
    uint8_t reg1[1], is_enable = 0;
    while (!is_enable) {
      read_register(0x05, reg1, 1);
      is_enable = ((reg1[0]) & 0x02);
      delay(1);
    }
    return true;
  }

  /// Software Reset
  bool reset() {
    uint8_t status = FLASH_OK;
    write_cmd(0x66);
    wait();
    status = write_cmd(0x99);
    wait();
    return status == FLASH_OK;
  }
};