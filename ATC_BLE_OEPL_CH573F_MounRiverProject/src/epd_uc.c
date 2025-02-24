#include "tl_common.h"

const uint8_t EPD_UC_color_change_table[2] = {0x10,0x13};

uint8_t EPD_UC_Display_start(uint8_t full_or_partial)
{
	printf("EPD_Display_start UC\r\n");
    // power on
    EPD_WriteCmd(0x04);
    EPD_CheckStatus(100);

    switch(settings.screen_type)
    {
    /*case DEVICE_TYPE_HS_BW_213_UC:
        // Booster soft start
        EPD_WriteCmd(0x06);
        EPD_WriteData(0x17);
        EPD_WriteData(0x17);
        EPD_WriteData(0x17);
        // resolution setting
        EPD_WriteCmd(0x61);
        EPD_WriteData(0x80);
        EPD_WriteData(0x01);
        EPD_WriteData(0x28);

        EPD_WriteCmd(0x50);
        EPD_WriteData(0x97);

        EPD_WriteCmd(0x00);
        EPD_WriteData((SCAN_UP | RES_160x296 | FORMAT_BW | BOOSTER_ON | RESET_NONE | LUT_OTP | SHIFT_RIGHT));
        EPD_WriteData(0x0f);
    	break;
    case DEVICE_TYPE_HS_BW_350_UC:
        EPD_WriteCmd(0x4D);
        EPD_WriteData(0x55);

        EPD_WriteCmd(0xF3);
        EPD_WriteData(0x0A);

        EPD_WriteCmd(0xE9);
        EPD_WriteData(0x02);

        EPD_WriteCmd(0x50);
        EPD_WriteData(0x97);

        EPD_WriteCmd(0x00);
        EPD_WriteData((SCAN_UP | RES_160x296 | FORMAT_BW | BOOSTER_ON | RESET_NONE | LUT_OTP | SHIFT_RIGHT));
        EPD_WriteData(0x0f);
    	break;
    case DEVICE_TYPE_HS_BWY_750_UC:
    	EPD_WriteCmd(0x00);
    	EPD_WriteData((BOOSTER_ON | RESET_NONE));
        EPD_WriteCmd(0x50);
        EPD_WriteData(0x11);
    	break;*/
    default:
    	EPD_WriteCmd(0x00);
    	EPD_WriteData((SCAN_UP | RES_160x296 | FORMAT_BWR | BOOSTER_ON | RESET_NONE | LUT_OTP | SHIFT_RIGHT));
    	EPD_WriteData(0x0f);
    	break;
    }
    EPD_UC_Display_color_change(0);
    return 0;
}

void EPD_UC_Display_byte(uint8_t data)
{
    EPD_WriteData(data);
}

void EPD_UC_Display_color_change(uint8_t newColor)
{
    if(settings.screen_colors == 1)
    	newColor = 1;
	if(newColor < sizeof(EPD_UC_color_change_table))
	{
	    EPD_WriteCmd(EPD_UC_color_change_table[newColor]);
	}
}

void EPD_UC_Display_end(void)
{
    EPD_WriteCmd(0x12);
}

uint8_t EPD_UC_busy_check(void)
{
    return GPIOA_ReadPortPin (EPD_BUSY) != 0;
}

void EPD_UC_set_sleep(void)
{
	printf("EPD_UC_set_sleep UC\r\n");
    // power off
    EPD_WriteCmd(0x02);
    // deep sleep
    EPD_WriteCmd(0x07);
    EPD_WriteData(0xa5);
}
