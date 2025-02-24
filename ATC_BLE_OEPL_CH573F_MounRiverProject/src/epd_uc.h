#pragma once

enum PSR_FLAGS
{
    RES_96x230 = 0b00000000,
    RES_96x252 = 0b01000000,
    RES_128x296 = 0b10000000,
    RES_160x296 = 0b11000000,

    LUT_OTP = 0b00000000,
    LUT_REG = 0b00100000,

    FORMAT_BWR = 0b00000000,
    FORMAT_BW = 0b00010000,

    SCAN_DOWN = 0b00000000,
    SCAN_UP = 0b00001000,

    SHIFT_LEFT = 0b00000000,
    SHIFT_RIGHT = 0b00000100,

    BOOSTER_OFF = 0b00000000,
    BOOSTER_ON = 0b00000010,

    RESET_SOFT = 0b00000000,
    RESET_NONE = 0b00000001
};

uint8_t EPD_UC_Display_start(uint8_t full_or_partial);
void EPD_UC_Display_byte(uint8_t data);
void EPD_UC_Display_color_change(uint8_t newColor);
uint8_t EPD_UC_busy_check(void);
void EPD_UC_Display_end(void);
void EPD_UC_set_sleep(void);
