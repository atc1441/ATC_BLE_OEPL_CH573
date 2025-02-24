#pragma once

#define EEPROM_IMG_START (0x00000UL)
#define EEPROM_IMG_LEN (0x80000UL)

#define EEPROM_WRITE_PAGE_SZ (256)
#define EEPROM_PAGE_SIZEour (0x01000)
void eepromRead (uint32_t addr, uint8_t *dst, uint32_t len);

uint8_t eepromWrite (uint32_t addr, uint8_t *src, uint32_t len);

uint8_t eepromErase (uint32_t addr, uint32_t len);

uint32_t eepromGetSize (void);

#define EEPROM_IMG_VALID (0x494d4721UL)

struct EepromImageHeader {  // each image space is 0x17000 bytes, we have space for ten of them
    uint64_t version;
    uint32_t validMarker;
    uint32_t size;
    uint8_t dataType;
    uint32_t id;

    // image data here
    // we pre-erase so progress can be calculated by finding the first non-0xff byte
};

void FLASH_Init();
void SPI_Flash_CS_set (int a1);
void SPI_Flash_CLK_set (int a1);
void SPI_Flash_MOSI_set (int a1);
uint8_t SPI_Flash_MISO_get();
void SPI_Flash_enable_GPIO();
void SPI_Flash_disable_GPIO();
void SPI_Flash_CS_select();
void SPI_Flash_CS_deselect();
uint8_t SPI_Flash_transceive (int a1);
void Flash_wait_busy();
void Flash_CMD_WriteEN();
void EPD_read_buffer (int address, uint8_t *a2, unsigned int a3);
void Flash_BlkErase_4k (int address);
void Flash_write_buffer (int address, uint8_t *a2, unsigned int a3);
