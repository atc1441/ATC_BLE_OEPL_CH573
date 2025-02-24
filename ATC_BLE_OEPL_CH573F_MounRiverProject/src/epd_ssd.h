#pragma once

uint8_t EPD_SSD_Display_start(uint8_t full_or_partial);
void EPD_SSD_Display_byte(uint8_t data);
void EPD_SSD_Display_color_change(uint8_t newColor);
uint8_t EPD_SSD_busy_check(void);
void EPD_SSD_Display_end(void);
void EPD_SSD_set_sleep(void);
