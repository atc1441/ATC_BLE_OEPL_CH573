#pragma once
#include "CH57x_common.h"
#include "config.h"
#include "HAL.h"


void EPD_Init();
void EPD_Display_start (uint8_t full_or_partial);
void EPD_CheckStatus (int max_ms);
void EPD_CheckStatus_inverted (int max_ms);
void EPD_SPI_Write (unsigned char value);
void EPD_WriteCmd (unsigned char cmd);
void EPD_WriteData (unsigned char data);
void EPD_Display_byte (uint8_t data);
void EPD_Display_color_change (uint8_t newColor);
void EPD_Display_end();
void epd_set_sleep (void);
uint8_t epd_state_handler (void);
