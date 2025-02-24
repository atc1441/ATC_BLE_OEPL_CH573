#include "tl_common.h"

uint8_t theColorState = 0;
uint8_t EPD_JD_Display_start(uint8_t full_or_partial)
{
	printf("EPD_Display_start JD\r\n");
	DelayMs(40);
    EPD_CheckStatus(400);

    EPD_WriteCmd(0x40);
    EPD_CheckStatus(400);
    EPD_WriteData(0x17);
    EPD_WriteData(0xC0);
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x06);
    EPD_WriteData(0x0A);
    EPD_WriteData(0x2F);
    EPD_WriteData(0x25);
    EPD_WriteData(0x22);
    EPD_WriteData(0x2E);
    EPD_WriteData(0x21);


    /////////////////
	EPD_WriteCmd(0x66);
	EPD_WriteData(0x49);
	EPD_WriteData(0x55);
	EPD_WriteData(0x13);
	EPD_WriteData(0x5D);
	EPD_WriteData(0x05);
	EPD_WriteData(0x10);


	EPD_WriteCmd(0x4D);
	EPD_WriteData(0x78);


	EPD_WriteCmd(0x00); //PSR
	EPD_WriteData(0x0F);
	EPD_WriteData(0x29);



	EPD_WriteCmd(0x01); //PWR
	EPD_WriteData(0x07);
	EPD_WriteData(0x00);


	EPD_WriteCmd(0x03); //POFS
	EPD_WriteData(0x10);
	EPD_WriteData(0x54);
	EPD_WriteData(0x44);

	EPD_WriteCmd(0x50);//CDI
	EPD_WriteData(0x37);

	EPD_WriteCmd(0x60); //TCON
	EPD_WriteData(0x02);
	EPD_WriteData(0x02);

	EPD_WriteCmd(0x61); //TRES
	EPD_WriteData(settings.screen_w/256);		// Source_BITS_H
	EPD_WriteData(settings.screen_w%256);		// Source_BITS_L
	EPD_WriteData(settings.screen_h/256);		// Gate_BITS_H
	EPD_WriteData(settings.screen_h%256); 		// Gate_BITS_L


	EPD_WriteCmd(0xE7);
	EPD_WriteData(0x1C);

	EPD_WriteCmd(0xE3); //PWS
	EPD_WriteData(0x22);

	EPD_WriteCmd(0xB6);
	EPD_WriteData(0x6F);

	EPD_WriteCmd(0xB4);
	EPD_WriteData(0xD0);

	EPD_WriteCmd(0xE9);
	EPD_WriteData(0x01);

	EPD_WriteCmd(0x30);// frame go with waveform
	EPD_WriteData(0x08);
	////////////////////////

    EPD_WriteCmd(0x04);
    EPD_CheckStatus(400);

    EPD_JD_Display_color_change(0);
    return 0;
}

void EPD_JD_Display_byte(uint8_t data)
{
	unsigned char data_H1,data_H2,data_L1,data_L2;
	if(theColorState==0)
	{
	      data_H1=(((data>>7)&1)?1:0)<<6;
	      data_H2=(((data>>6)&1)?1:0)<<4;
	      data_L1=(((data>>5)&1)?1:0)<<2;
	      data_L2=(((data>>4)&1)?1:0);
	      EPD_WriteData(data_H1|data_H2|data_L1|data_L2);
	      data_H1=(((data>>3)&1)?1:0)<<6;
	      data_H2=(((data>>2)&1)?1:0)<<4;
	      data_L1=(((data>>1)&1)?1:0)<<2;
	      data_L2=(((data>>0)&1)?1:0);
	      EPD_WriteData(data_H1|data_H2|data_L1|data_L2);
	}/*else if(theColorState==1){
	      data_H1=(((data>>7)&1)?2:0)<<6;
	      data_H2=(((data>>6)&1)?2:0)<<4;
	      data_L1=(((data>>5)&1)?2:0)<<2;
	      data_L2=(((data>>4)&1)?2:0);
	      EPD_WriteData(data_H1|data_H2|data_L1|data_L2);
	      data_H1=(((data>>3)&1)?2:0)<<6;
	      data_H2=(((data>>2)&1)?2:0)<<4;
	      data_L1=(((data>>1)&1)?2:0)<<2;
	      data_L2=(((data>>0)&1)?2:0);
	      EPD_WriteData(data_H1|data_H2|data_L1|data_L2);
	}else if(theColorState==2){
	      data_H1=(((data>>7)&1)?3:0)<<6;
	      data_H2=(((data>>6)&1)?3:0)<<4;
	      data_L1=(((data>>5)&1)?3:0)<<2;
	      data_L2=(((data>>4)&1)?3:0);
	      EPD_WriteData(data_H1|data_H2|data_L1|data_L2);
	      data_H1=(((data>>3)&1)?3:0)<<6;
	      data_H2=(((data>>2)&1)?3:0)<<4;
	      data_L1=(((data>>1)&1)?3:0)<<2;
	      data_L2=(((data>>0)&1)?3:0);
	      EPD_WriteData(data_H1|data_H2|data_L1|data_L2);
	}*/
}

void EPD_JD_Display_color_change(uint8_t newColor)
{
	theColorState = newColor;
	if(theColorState == 0)
		EPD_WriteCmd(0x10);
}

void EPD_JD_Display_end(void)
{
    EPD_WriteCmd(0x12);
    EPD_WriteData(0x00);
}

uint8_t EPD_JD_busy_check(void)
{
    return gpio_read(settings.epd_pinout->BUSY) != 0;
}

void EPD_JD_set_sleep(void)
{
	printf("EPD_UC_set_sleep JD\r\n");
    // power off
    EPD_WriteCmd(0x02);
    EPD_WriteData(0x00);
    EPD_CheckStatus(400);
    // deep sleep
    EPD_WriteCmd(0x07);
    EPD_WriteData(0xa5);
}
