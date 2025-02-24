#include "tl_common.h"


uint8_t EPD_SSD_Display_start(uint8_t full_or_partial)
{
	printf("EPD_Display_start SSD\r\n");
    // SW Reset
    EPD_WriteCmd(0x12);
    EPD_CheckStatus_inverted(100);

    EPD_WriteCmd(0x11);
    EPD_WriteData(0x03);

    if(settings.screen_w_h_inversed){
        EPD_WriteCmd(0x44);
        EPD_WriteData(((settings.screen_w_offset)/8)&0xff);
        EPD_WriteData((((settings.screen_w_offset+settings.screen_w)-8)/8)&0xff);

        EPD_WriteCmd(0x45);
        EPD_WriteData(settings.screen_h_offset&0xff);
        EPD_WriteData((settings.screen_h_offset>>8)&0xff);
        EPD_WriteData(((settings.screen_h_offset+settings.screen_h)-1)&0xff);
        EPD_WriteData((((settings.screen_h_offset+settings.screen_h)-1)>>8)&0xff);
    }else{
        EPD_WriteCmd(0x44);
        EPD_WriteData(((settings.screen_h_offset)/8)&0xff);
        EPD_WriteData((((settings.screen_h_offset+settings.screen_h)-8)/8)&0xff);

        EPD_WriteCmd(0x45);
        EPD_WriteData(settings.screen_w_offset&0xff);
        EPD_WriteData((settings.screen_w_offset>>8)&0xff);
        EPD_WriteData(((settings.screen_w_offset+settings.screen_w)-1)&0xff);
        EPD_WriteData((((settings.screen_w_offset+settings.screen_w)-1)>>8)&0xff);
    }

    EPD_WriteCmd(0x3c);
    EPD_WriteData(0x05);

    EPD_WriteCmd(0x18);
    EPD_WriteData(0x80);

    EPD_WriteCmd(0x22);
    EPD_WriteData(0xB1);

    EPD_WriteCmd(0x20);
    EPD_CheckStatus_inverted(100);
    EPD_SSD_Display_color_change(0);
    return 0;
}

void EPD_SSD_Display_byte(uint8_t data)
{
    EPD_WriteData(data);
}

const uint8_t EPD_SSD_color_change_table[2] = {0x24,0x26};
void EPD_SSD_Display_color_change(uint8_t newColor)
{
	if(newColor < sizeof(EPD_SSD_color_change_table))
	{
	    if(settings.screen_w_h_inversed){
	        EPD_WriteCmd(0x4E);
	        EPD_WriteData(((settings.screen_w_offset)/8)&0xff);

	        EPD_WriteCmd(0x4F);
	        EPD_WriteData(settings.screen_h_offset&0xff);
	        EPD_WriteData((settings.screen_h_offset>>8)&0xff);
	    }else{
	        EPD_WriteCmd(0x4E);
	        EPD_WriteData(((settings.screen_h_offset)/8)&0xff);

	        EPD_WriteCmd(0x4F);
	        EPD_WriteData(settings.screen_w_offset&0xff);
	        EPD_WriteData((settings.screen_w_offset>>8)&0xff);
	    }
	    EPD_WriteCmd(EPD_SSD_color_change_table[newColor]);
	}
}

void EPD_SSD_Display_end(void)
{
    // Display update control
    EPD_WriteCmd(0x22);
    //if(settings.screen_type == DEVICE_TYPE_HS_BWR_420_SSD || settings.screen_type == DEVICE_TYPE_GICI_BWR_420_SSD)
    //	EPD_WriteData(0xC7);
    //else
        EPD_WriteData(0xCF);
    // Master Activation
    EPD_WriteCmd(0x20);
	DelayMs(10);
}

uint8_t EPD_SSD_busy_check(void)
{
    return GPIOA_ReadPortPin (EPD_BUSY) == 0;
}

void EPD_SSD_set_sleep(void)
{
	printf("EPD_SSD_set_sleep SSD\r\n");
    EPD_WriteCmd(0x10);
    EPD_WriteData(0x01);
}
