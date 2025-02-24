#include "tl_common.h"

#define EPD_MASTER 1
#define EPD_SLAVE 2

#define EPD_BWR_970_test_pattern 0xA5

RAM uint32_t epd_byte_len;

RAM uint8_t epdOTPbuffer[128] = {0};
RAM uint32_t byte_counts = 0;
RAM uint32_t byte_state = 0;
RAM uint8_t byteOrientation = 0;

void epd_start(uint8_t mode)
{
    if ((mode & EPD_MASTER) != 0)
        gpio_write(settings.epd_pinout->CS, 0);
    if ((mode & EPD_SLAVE) != 0)
        gpio_write(settings.epd_pinout->CSs, 0);
}

void epd_stop(uint8_t mode)
{
    if ((mode & EPD_MASTER) != 0)
        gpio_write(settings.epd_pinout->CS, 1);
    if ((mode & EPD_SLAVE) != 0)
        gpio_write(settings.epd_pinout->CSs, 1);
}
void epd_cmd(uint8_t data)
{
    gpio_write(settings.epd_pinout->DC, 0);
    EPD_SPI_Write(data);
    gpio_write(settings.epd_pinout->DC, 1);
}

void epd_data(uint8_t data)
{
    EPD_SPI_Write(data);
}

void epd_wait_busy(uint8_t mode)
{
    if ((mode & EPD_MASTER) != 0)
    {
        printf("Now master busy\r\n");
        while (gpio_read(settings.epd_pinout->BUSY) == 0)
        {
            wdt10s();
        }
    }
    if ((mode & EPD_SLAVE) != 0)
    {
        printf("Now slave busy\r\n");
        while (gpio_read(settings.epd_pinout->BUSYs) == 0)
        {
            wdt10s();
        }
    }
}

uint8_t readEPDbyte()
{
    uint8_t temp = 0x00;
    WaitUs(10);
    for (uint32_t i = 0; i < 8; i++)
    {
        gpio_write(settings.epd_pinout->CLK, 0);
        gpio_write(settings.epd_pinout->CLK, 1);
        temp <<= 1;
        if (gpio_read(settings.epd_pinout->MOSI) != 0)
        {
            temp |= 1;
        }
    }
    return temp;
}

void readFromEPD(uint8_t *byteBuffer, uint32_t lenRead)
{
    epd_start(EPD_MASTER);
    if (settings.screen_type == DEVICE_TYPE_TI_BWR_1200_V2) // Connected screen is 12" V2
    {
        epd_cmd(0xA1);
        epd_data(0x00);
        epd_stop(EPD_MASTER);
        epd_start(EPD_MASTER);
        epd_cmd(0xA7);
        epd_data(0x80);
        epd_stop(EPD_MASTER);
        epd_start(EPD_MASTER);
        epd_cmd(0xA3);
        epd_data(0x00);
        epd_data(0x40);
        epd_stop(EPD_MASTER);
        epd_start(EPD_MASTER);
    	epd_cmd(0xA8);
    }else
    {
    	epd_cmd(0xB9);
    }
    gpio_write(settings.epd_pinout->DC, 1);
    gpio_shutdown(settings.epd_pinout->MOSI);
    gpio_set_output_en(settings.epd_pinout->MOSI, 0);
    gpio_set_input_en(settings.epd_pinout->MOSI, 1);
    readEPDbyte(); // Dummy byte
    for (uint32_t i = 0; i < lenRead; i++)
    {
        byteBuffer[i] = readEPDbyte();
    }

    gpio_set_output_en(settings.epd_pinout->MOSI, 1);
    gpio_set_input_en(settings.epd_pinout->MOSI, 0);
    epd_stop(EPD_MASTER);
}

uint8_t EPD_TI_Display_start(uint8_t full_or_partial)
{
	printf("EPD_Display_start TI\r\n");

    WaitUs(100);
    uint8_t forcedByteOrientation = 0;
    readFromEPD(epdOTPbuffer, sizeof(epdOTPbuffer));
    printf("Read OTP:\r\n");
    for (uint32_t i = 0; i < 10; i++)
    {
        printf("%02X ", epdOTPbuffer[i]);
    }
    printf("\r\n");
    if (settings.screen_type == DEVICE_TYPE_TI_BWR_1200_V2) // Connected screen is 12" V2
    {
        printf("Setting to 12 V2 Screen\r\n");
        epdOTPbuffer[0x10] = 0x09;

        epdOTPbuffer[0x15] = 0x00;
        epdOTPbuffer[0x16] = 0x3B;
        epdOTPbuffer[0x17] = 0x00;
        epdOTPbuffer[0x18] = 0x00;
        epdOTPbuffer[0x19] = 0xFF;
        epdOTPbuffer[0x1A] = 0x02;

        epdOTPbuffer[0x0C] = 0x00;
        epdOTPbuffer[0x0D] = 0x3B;
        epdOTPbuffer[0x0E] = 0x00;
        epdOTPbuffer[0x0F] = 0xC1;

        epdOTPbuffer[0x12] = 0x00;
        epdOTPbuffer[0x13] = 0x00;
        epdOTPbuffer[0x14] = 0x10;

        epdOTPbuffer[0x1C] = 0x80;
        epdOTPbuffer[0x1D] = 0x00;

        epdOTPbuffer[0x0B] = 0x25;
        epdOTPbuffer[0x1B] = 0x01;
        epdOTPbuffer[0x11] = 0x00;
        forcedByteOrientation = 1;
    }
    else if ((epdOTPbuffer[0x19] == 0xff && epdOTPbuffer[0x1A] == 0x02) || (settings.screen_type == DEVICE_TYPE_TI_BWR_1200)) // Connected screen is 12"
    {
        printf("Setting to 12 Screen\r\n");
    }
    else
    { // Connected screen is 9.7"
        printf("Setting to 9.7 Screen\r\n");
        if (epdOTPbuffer[0x15] == 0xff && epdOTPbuffer[0x16] == 0xff && epdOTPbuffer[0x17] == 0xff && epdOTPbuffer[0x18] == 0xff && epdOTPbuffer[0x19] == 0xff && epdOTPbuffer[0x1A] == 0xff)
        {
            printf("Error reading OTP forcing 9.7 Screen\r\n");
            epdOTPbuffer[0x10] = 0x09;

            epdOTPbuffer[0x15] = 0x00;
            epdOTPbuffer[0x16] = 0x3B;
            epdOTPbuffer[0x17] = 0x00;
            epdOTPbuffer[0x18] = 0x00;
            epdOTPbuffer[0x19] = 0x9F;
            epdOTPbuffer[0x1A] = 0x02;

            epdOTPbuffer[0x0C] = 0x00;
            epdOTPbuffer[0x0D] = 0x3B;
            epdOTPbuffer[0x0E] = 0x00;
            epdOTPbuffer[0x0F] = 0xA8;

            epdOTPbuffer[0x12] = 0x00;
            epdOTPbuffer[0x13] = 0x00;
            epdOTPbuffer[0x14] = 0x00;

            epdOTPbuffer[0x1C] = 0x80;
            epdOTPbuffer[0x1D] = 0x00;

            epdOTPbuffer[0x0B] = 0x25;
            epdOTPbuffer[0x1B] = 0x01;
            epdOTPbuffer[0x11] = 0x00;
            forcedByteOrientation = 1;
        }
    }
    epd_byte_len = (((settings.screen_w)*settings.screen_h) / 8);
    if(forcedByteOrientation)
        byteOrientation = 1;
    WaitMs(200);
    gpio_write(settings.epd_pinout->RESET, 1);
    WaitMs(20);
    gpio_write(settings.epd_pinout->RESET, 0);
    WaitMs(200);
    gpio_write(settings.epd_pinout->RESET, 1);
    WaitMs(200);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x01);
    epd_data(epdOTPbuffer[0x10]);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x13);
    epd_data(epdOTPbuffer[0x15]);
    epd_data(epdOTPbuffer[0x16]);
    epd_data(epdOTPbuffer[0x17]);
    epd_data(epdOTPbuffer[0x18]);
    epd_data(epdOTPbuffer[0x19]);
    epd_data(epdOTPbuffer[0x1A]);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x90);
    epd_data(epdOTPbuffer[0x0C]);
    epd_data(epdOTPbuffer[0x0D]);
    epd_data(epdOTPbuffer[0x0E]);
    epd_data(epdOTPbuffer[0x0F]);
    epd_stop(EPD_MASTER | EPD_SLAVE);

    epd_start(EPD_MASTER | EPD_SLAVE); // Send color 1
    epd_cmd(0x12);
    epd_data(epdOTPbuffer[0x12]);
    epd_data(epdOTPbuffer[0x13]);
    epd_data(epdOTPbuffer[0x14]);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x10);
    epd_stop(EPD_SLAVE);
    byte_counts = 0;
    byte_state = 0;
    return 0;
}

uint8_t reverseBits(uint8_t byte)
{
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return byte;
}
extern uint8_t onlineState;
void EPD_TI_Display_byte(uint8_t data)
{
    if (byteOrientation)
        data = reverseBits(data);
    if (onlineState == 0 && (byte_state == 0) && ((byte_counts % 120) == 0))
    {
        epd_data(0x55);
    }
    else
    {
        epd_data(data);
    }
    byte_counts++;
    if ((byte_counts % 120) == 60)
    {
        epd_stop(EPD_MASTER);
        epd_start(EPD_SLAVE);
    }
    else if ((byte_counts % 120) == 0)
    {
        epd_stop(EPD_SLAVE);
        epd_start(EPD_MASTER);
    }
    if (byte_counts >= epd_byte_len)
    {
        byte_counts = 0;
        printf("Panel change\r\n");
        switch (byte_state)
        {
        default:
        case 0:
            epd_stop(EPD_SLAVE);

            epd_start(EPD_MASTER | EPD_SLAVE); // Send Color 2
            epd_cmd(0x12);
            epd_data(epdOTPbuffer[0x12]);
            epd_data(epdOTPbuffer[0x13]);
            epd_data(epdOTPbuffer[0x14]);
            epd_stop(EPD_MASTER | EPD_SLAVE);
            epd_start(EPD_MASTER | EPD_SLAVE);
            epd_cmd(0x11);
            epd_stop(EPD_SLAVE);
            byte_state++;
            break;
        case 1:
            epd_stop(EPD_SLAVE);
            byte_state = 0;
            break;
        }
    }
}

void EPD_TI_Display_color_change(uint8_t newColor)
{
}

void EPD_TI_Display_end(void)
{
	uint8_t temperature = 20;
    ////// Refresh starting
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x05);
    epd_data(0x7D);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    WaitMs(200);

    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x05);
    epd_data(0x00);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    WaitMs(10);
/////////////////////////////////////////////////////////
    if (settings.screen_type == DEVICE_TYPE_TI_BWR_1200_V2) // Connected screen is 12" V2
    {
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0xC2);
    epd_data(0x3F);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x03);
    epd_data(0x00);
    epd_data(0x12);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    }
    /////////////////////////////////////////////////////////
    WaitMs(10);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0xD8);
    epd_data(epdOTPbuffer[0x1C]);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0xD6);
    epd_data(epdOTPbuffer[0x1D]);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0xA7);
    epd_data(0x10);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    // epd_wait_busy(EPD_MASTER | EPD_SLAVE);
    WaitMs(100);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0xA7);
    epd_data(0x00);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    WaitMs(100);
////////////////////////////////////////////////////////
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x44);
    epd_data(0x00);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x45);
    epd_data(0x80);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0xA7);
    epd_data(0x10);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    // epd_wait_busy(EPD_MASTER | EPD_SLAVE);
    WaitMs(100);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0xA7);
    epd_data(0x00);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    WaitMs(100);

    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x44);
    epd_data(0x06);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x45);
    epd_data((uint8_t)((temperature + 40) * 2));
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0xA7);
    epd_data(0x10);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    // epd_wait_busy(EPD_MASTER | EPD_SLAVE);
    WaitMs(100);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0xA7);
    epd_data(0x00);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    WaitMs(100);
/////////////////////////////////////////////////////////

    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x60);
    epd_data(epdOTPbuffer[0x0B]);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER);
    epd_cmd(0x61);
    epd_data(epdOTPbuffer[0x1B]);
    epd_stop(EPD_MASTER);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x02);
    epd_data(epdOTPbuffer[0x11]);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    for (int i = 0; i < 4; i++)
    {
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x09);
        epd_data(0x1F);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x51);
        epd_data(0x50);
        epd_data(i + 1);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x09);
        epd_data(0x9F);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        WaitMs(1);
    }
    for (int i = 0; i < 10; i++)
    {
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x09);
        epd_data(0x1F);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x51);
        epd_data(0x0A);
        epd_data(i + 1);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x09);
        epd_data(0x9F);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        WaitMs(1);
    }
    for (int i = 0; i < 10; i++)
    {
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x09);
        epd_data(0x7F);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x51);
        epd_data(0x0A);
        epd_data(i + 3);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x09);
        epd_data(0xFF);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        WaitMs(3);
    }
    for (int i = 0; i < 7; i++)
    {
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x09);
        epd_data(0x7F);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x51);
        epd_data(0x09 - i);
        epd_data(0x0C);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        epd_start(EPD_MASTER | EPD_SLAVE);
        epd_cmd(0x09);
        epd_data(0xFF);
        epd_stop(EPD_MASTER | EPD_SLAVE);
        WaitMs(1);
    }
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x15);
    epd_data(0x3C);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    printf("Refresh start\r\n");
}

uint8_t EPD_TI_busy_check(void)
{
    return gpio_read(settings.epd_pinout->BUSY) != 0;
}

void EPD_TI_set_sleep(void)
{
	printf("EPD_SSD_set_sleep TI\r\n");
    WaitMs(10);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x09);
    epd_data(0x7F);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x05);
    epd_data(0x3D);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x09);
    epd_data(0x7E);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    WaitMs(20);
    epd_start(EPD_MASTER | EPD_SLAVE);
    epd_cmd(0x09);
    epd_data(0x00);
    epd_stop(EPD_MASTER | EPD_SLAVE);
    WaitMs(20);
}
