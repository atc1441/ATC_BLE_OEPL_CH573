#include "epd.h"
#include "CH57x_common.h"
#include "config.h"
#include "HAL.h"
#include "main.h"

void EPD_Init() {
    GPIOA_ModeCfg (EPD_DC, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg (EPD_RESET, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg (EPD_BUSY, GPIO_ModeIN_PU);
    GPIOA_ModeCfg (EPD_MOSI, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg (EPD_CLK, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg (EPD_CS, GPIO_ModeOut_PP_5mA);

    GPIOA_SetBits (EPD_CS);

    DelayMs (5);
    GPIOA_ResetBits (EPD_RESET);
    DelayMs (20);
    GPIOA_SetBits (EPD_RESET);
    DelayMs (20);
}

void EPD_SPI_Write (unsigned char value) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        GPIOA_ResetBits (EPD_CLK);
        if (value & 0x80) {
            GPIOA_SetBits (EPD_MOSI);
        } else {
            GPIOA_ResetBits (EPD_MOSI);
        }
        value = (value << 1);
        GPIOA_SetBits (EPD_CLK);
    }
    GPIOA_ResetBits (EPD_CLK);
}

void EPD_WriteCmd (unsigned char cmd) {
    GPIOA_ResetBits (EPD_CS);
    GPIOA_ResetBits (EPD_DC);
    EPD_SPI_Write (cmd);
    GPIOA_SetBits (EPD_CS);
}

void EPD_WriteData (unsigned char data) {
    GPIOA_ResetBits (EPD_CS);
    GPIOA_SetBits (EPD_DC);
    EPD_SPI_Write (data);
    GPIOA_SetBits (EPD_CS);
}