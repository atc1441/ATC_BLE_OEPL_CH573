#include "tl_common.h"


uint32_t bytePosi = 0;
uint8_t EPD_ST_Display_start(uint8_t full_or_partial)
{
	printf("EPD_Display_start ST\r\n");
    EPD_WriteCmd(0xD6);
    EPD_WriteData(0x17);
    EPD_WriteData(0x02);
    EPD_WriteCmd(0xD1);
    EPD_WriteData(0x01);
    EPD_WriteCmd(0xC0);
    EPD_WriteData(0x0E);
    EPD_WriteData(0x05);
    EPD_WriteCmd(0xC1);
    EPD_WriteData(0x41);
    EPD_WriteData(0x41);
    EPD_WriteData(0x41);
    EPD_WriteData(0x41);
    EPD_WriteCmd(0xC2);
    EPD_WriteData(0x32);
    EPD_WriteData(0x32);
    EPD_WriteData(0x32);
    EPD_WriteData(0x32);
    EPD_WriteCmd(0xC4);
    EPD_WriteData(0x4B);
    EPD_WriteData(0x4B);
    EPD_WriteData(0x4B);
    EPD_WriteData(0x4B);
    EPD_WriteCmd(0xC5);
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);
    EPD_WriteCmd(0xD8);
    EPD_WriteData(0xA6);
    EPD_WriteData(0xE9);
    EPD_WriteCmd(0xB2);
    EPD_WriteData(0x11);
    EPD_WriteCmd(0xB3);
    EPD_WriteData(0xE5);
    EPD_WriteData(0xF6);
    EPD_WriteData(0x05);
    EPD_WriteData(0x46);
    EPD_WriteData(0x77);
    EPD_WriteData(0x77);
    EPD_WriteData(0x77);
    EPD_WriteData(0x77);
    EPD_WriteData(0x76);
    EPD_WriteData(0x45);
    EPD_WriteCmd(0xB4);
    EPD_WriteData(0x05);
    EPD_WriteData(0x46);
    EPD_WriteData(0x77);
    EPD_WriteData(0x77);
    EPD_WriteData(0x77);
    EPD_WriteData(0x77);
    EPD_WriteData(0x76);
    EPD_WriteData(0x45);
    EPD_WriteCmd(0xB7);
    EPD_WriteData(0x13);
    EPD_WriteCmd(0xB0);
    EPD_WriteData(0x3F);
    EPD_WriteCmd(0x11);
    WaitMs(120);
    EPD_WriteCmd(0xC9);
    EPD_WriteData(0x00);
    EPD_WriteCmd(0xC7);
    EPD_WriteData(0xC1);
    EPD_WriteData(0x41);
    EPD_WriteData(0x26);
    EPD_WriteCmd(0x36);
    EPD_WriteData(0x80);
    EPD_WriteCmd(0x3A);
    EPD_WriteData(0x11);
    EPD_WriteCmd(0xB9);
    EPD_WriteData(0x20);
    EPD_WriteCmd(0xB8);
    EPD_WriteData(0x25);
    EPD_WriteCmd(0x21);
    EPD_WriteCmd(0x2A);
    EPD_WriteData(0x23);
    EPD_WriteData(0x41);
    EPD_WriteCmd(0x2B);
    EPD_WriteData(0x7C);
    EPD_WriteData(0x41);
    EPD_WriteCmd(0x35);
    EPD_WriteData(0x00);
    EPD_WriteCmd(0xD0);
    EPD_WriteData(0xFF);
    EPD_WriteCmd(0x39);
    EPD_WriteCmd(0x29);
    EPD_WriteCmd(0x28);
    EPD_WriteCmd(0x2A);
    EPD_WriteData(0x19);
    EPD_WriteData(0x23);
    EPD_WriteCmd(0x2B);
    EPD_WriteData(0x00);
    EPD_WriteData(0x7C);
    EPD_WriteCmd(0x2C);
    bytePosi = 0;
    return 0;
}

static inline int getBit(const unsigned char *array, int index)
{
    return (array[index / 8] >> (7 - (index % 8))) & 1;
}

static inline void setBit(unsigned char *array, int index, int value)
{
    if (value)
        array[index / 8] |= (1 << (7 - (index % 8)));
    else
        array[index / 8] &= ~(1 << (7 - (index % 8)));
}

void sortPixels(unsigned char *inputArray, unsigned char *outputArray)
{
    for (int i = 0; i < 264; i++)
    {
        if (i < 132)
        {
            int bit = getBit(inputArray, i);
            setBit(outputArray, (i * 2) + 1, bit);
        }
        else
        {
            int bit = getBit(inputArray, i + 4);
            setBit(outputArray, ((i - 132) * 2), bit);
        }
    }
}

uint8_t mClutMap[34];
uint8_t mClutMap1[34];
void EPD_ST_Display_byte(uint8_t data)
{
    mClutMap[bytePosi++] = data;
    if (bytePosi >= 34)
    {
        sortPixels(mClutMap, mClutMap1);
        for (int i = 0; i < 33; i++)
        {
        	EPD_WriteData(mClutMap1[i]);
        }
        bytePosi = 0;
    }
}

void EPD_ST_Display_color_change(uint8_t newColor)
{
}


RAM uint32_t start_time = 0;
void EPD_ST_Display_end(void)
{
    EPD_WriteCmd(0x39);
    EPD_WriteCmd(0x29);
    WaitMs(100);
    start_time = getMillis();
}

uint8_t EPD_ST_busy_check(void)
{
	return 1;
	if(getMillis() - start_time>= 4000)
		return 1;
	return 0;
}

void EPD_ST_set_sleep(void)
{
	printf("EPD_SSD_set_sleep SSD\r\n");
}
