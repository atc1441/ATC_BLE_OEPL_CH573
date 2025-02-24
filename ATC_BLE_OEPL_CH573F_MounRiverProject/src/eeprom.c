#include "tl_common.h"

uint8_t infoFlash[8];

void FLASH_Init() {
    SPI_Flash_enable_GPIO();
    DelayMs (20);
    unsigned int i;
    // SPI_Flash_CS_select();
    // SPI_Flash_transceive (0xab);
    // SPI_Flash_CS_deselect();
    SPI_Flash_CS_select();
    SPI_Flash_transceive (0x9F);

    PRINT ("Flash info: ");
    for (i = 0; i < 3; ++i) {
        infoFlash[i] = SPI_Flash_transceive (0xff);
        PRINT ("%02X ", infoFlash[i]);
    }
    PRINT ("\r\n");
    SPI_Flash_CS_deselect();
}

void SPI_Flash_CS_set (int a1) {
    if (a1)
        GPIOB_SetBits (SPI_FLASH_CS);
    else
        GPIOB_ResetBits (SPI_FLASH_CS);
}

void SPI_Flash_CLK_set (int a1) {
    if (a1)
        GPIOB_SetBits (SPI_FLASH_CLK);
    else
        GPIOB_ResetBits (SPI_FLASH_CLK);
}

void SPI_Flash_MOSI_set (int a1) {
    if (a1)
        GPIOB_SetBits (SPI_FLASH_MOSI);
    else
        GPIOB_ResetBits (SPI_FLASH_MOSI);
}

uint8_t SPI_Flash_MISO_get() {
    return GPIOB_ReadPortPin (SPI_FLASH_MISO) != 0;
}

void SPI_Flash_enable_GPIO() {
    GPIOB_ModeCfg (SPI_FLASH_CS, GPIO_ModeOut_PP_5mA);
    GPIOB_ModeCfg (SPI_FLASH_CLK, GPIO_ModeOut_PP_5mA);
    GPIOB_ModeCfg (SPI_FLASH_MISO, GPIO_ModeIN_Floating);
    GPIOB_ModeCfg (SPI_FLASH_MOSI, GPIO_ModeOut_PP_5mA);
    SPI_Flash_CS_set (1);
    SPI_Flash_CLK_set (0);
}

void SPI_Flash_disable_GPIO() {
    GPIOB_ModeCfg (SPI_FLASH_CS, GPIO_ModeIN_PU);
    GPIOB_ModeCfg (SPI_FLASH_CLK, GPIO_ModeIN_Floating);
    GPIOB_ModeCfg (SPI_FLASH_MISO, GPIO_ModeIN_Floating);
    GPIOB_ModeCfg (SPI_FLASH_MOSI, GPIO_ModeIN_Floating);
}

void SPI_Flash_CS_select() {
    SPI_Flash_CS_set (0);
}

void SPI_Flash_CS_deselect() {
    SPI_Flash_CS_set (1);
}

uint8_t SPI_Flash_transceive (int a1) {
    uint8_t v2 = 0;
    uint8_t v3 = 0;
    do {
        SPI_Flash_MOSI_set (((0x80u >> v3) & a1) != 0);
        SPI_Flash_CLK_set (1);
        if (SPI_Flash_MISO_get() == 1)
            v2 |= 0x80u >> v3;
        SPI_Flash_CLK_set (0);
        v3++;
    } while (v3 < 8);
    return v2;
}

void Flash_wait_busy() {
    int v0;
    SPI_Flash_CS_select();
    SPI_Flash_transceive (0x05);
    v0 = 1000000;
    if (SPI_Flash_transceive (0xff) & 1) {
        do
            --v0;
        while (v0 && SPI_Flash_transceive (0xff) & 1);
    }
    SPI_Flash_CS_deselect();
}

void Flash_CMD_WriteEN() {
    SPI_Flash_CS_select();
    SPI_Flash_transceive (0x06);
    SPI_Flash_CS_deselect();
}

void EPD_read_buffer (int address, uint8_t *a2, unsigned int a3) {
    unsigned int i;
    SPI_Flash_CS_select();
    SPI_Flash_transceive (0x03);
    SPI_Flash_transceive ((address >> 16) & 0xff);
    SPI_Flash_transceive ((address >> 8) & 0xff);
    SPI_Flash_transceive (address & 0xff);
    for (i = 0; i < a3; ++i)
        a2[i] = SPI_Flash_transceive (0xff);
    SPI_Flash_CS_deselect();
}

void Flash_BlkErase_4k (int address) {
    Flash_CMD_WriteEN();
    SPI_Flash_CS_select();
    SPI_Flash_transceive (0x20);
    SPI_Flash_transceive ((address >> 16) & 0xff);
    SPI_Flash_transceive ((address >> 8) & 0xff);
    SPI_Flash_transceive (address & 0xff);
    SPI_Flash_CS_deselect();
    Flash_wait_busy();
}

void Flash_write_buffer (int address, uint8_t *a2, unsigned int a3) {
    unsigned int i;
    Flash_CMD_WriteEN();
    SPI_Flash_CS_select();
    SPI_Flash_transceive (0x02);
    SPI_Flash_transceive ((address >> 16) & 0xff);
    SPI_Flash_transceive ((address >> 8) & 0xff);
    SPI_Flash_transceive (address & 0xff);
    for (i = 0; i < a3; i++)
        SPI_Flash_transceive (a2[i]);
    SPI_Flash_CS_deselect();
    Flash_wait_busy();
}

uint32_t eepromGetSize (void) {
    return EEPROM_IMG_LEN;
}

void eepromRead (uint32_t addr, uint8_t *dstP, uint32_t len) {
    printf ("Eeprom read %X Len: %d\r\n", addr, len);
    EPD_read_buffer (addr, dstP, len);
}

uint8_t eepromWrite (uint32_t addr, uint8_t *srcP, uint32_t len) {
    printf ("Eeprom write %X Len: %d\r\n", addr, len);
    while (len) {
        uint16_t lenNow = EEPROM_WRITE_PAGE_SZ - (addr & (EEPROM_WRITE_PAGE_SZ - 1));
        if (lenNow > len)
            lenNow = len;
        Flash_write_buffer (addr, srcP, lenNow);
        addr += lenNow;
        srcP += lenNow;
        len -= lenNow;
    }
    return 1;
}

uint8_t eepromErase (uint32_t addr, uint32_t len) {
    printf ("Eeprom erase %X Len: %d\r\n", addr, len);
    // round starting address down
    if (addr % EEPROM_PAGE_SIZEour) {
        len += addr % EEPROM_PAGE_SIZEour;
        addr = addr / EEPROM_PAGE_SIZEour * EEPROM_PAGE_SIZEour;
    }
    // round length up
    len = (len + EEPROM_PAGE_SIZEour - 1) / EEPROM_PAGE_SIZEour * EEPROM_PAGE_SIZEour;

    printf ("Checked Eeprom erase %X Len: %d\r\n", addr, len);
    while (len) {
        uint32_t now;
        Flash_BlkErase_4k (addr);
        now = 0x1000;

        addr += now;
        len -= now;
    }
    return 1;
}
