#include "tl_common.h"

#define LINE_BYTE_COUNTER ((settings.screen_w / 8) * 5)  // Draw 5 lines
extern char ownMacString[];

uint32_t byteCounter = 0;

void drawOnOffline (uint8_t state) {
    // onlineState = state;
}

static uint8_t mClutMap[256];

void drawImageAtAddress (uint32_t addr, uint8_t lut) {
    byteCounter = 0;
    struct EepromImageHeader *eih = (struct EepromImageHeader *)mClutMap;
    eepromRead (addr, mClutMap, sizeof (struct EepromImageHeader));
    switch (eih->dataType) {
    case DATATYPE_IMG_RAW_1BPP:
        printf ("Doing raw 1bpp\r\n");
        EPD_Display_start (1);
        for (uint32_t c = 0; c < ((settings.screen_h * settings.screen_w) / 8); c++) {
            if (c % 256 == 0) {
                eepromRead (addr + sizeof (struct EepromImageHeader) + c, mClutMap, 256);
            }
            EPD_Display_byte (settings.screen_color_black_invert ? ~mClutMap[c % 256] : mClutMap[c % 256]);
            byteCounter++;
        }
        if (settings.screen_colors > 1) {
            EPD_Display_color_change (1);
            for (uint32_t c = 0; c < ((settings.screen_h * settings.screen_w) / 8); c++) {
                EPD_Display_byte (settings.screen_color_second_invert ? ~0x00 : 0x00);
            }
        }
        EPD_Display_end();
        break;
    case DATATYPE_IMG_RAW_2BPP:
        printf ("Doing raw 2bpp\r\n");
        EPD_Display_start (1);
        for (uint32_t c = 0; c < ((settings.screen_h * settings.screen_w) / 8); c++) {
            if (c % 256 == 0) {
                eepromRead (addr + sizeof (struct EepromImageHeader) + c, mClutMap, 256);
            }
            EPD_Display_byte (settings.screen_color_black_invert ? ~mClutMap[c % 256] : mClutMap[c % 256]);
            byteCounter++;
        }
        if (settings.screen_colors > 1) {
            EPD_Display_color_change (1);
            for (uint32_t c = 0; c < ((settings.screen_h * settings.screen_w) / 8); c++) {
                if (c % 256 == 0) {
                    eepromRead (addr + sizeof (struct EepromImageHeader) + ((settings.screen_h * settings.screen_w) / 8) + c, mClutMap, 256);
                }
                EPD_Display_byte (settings.screen_color_second_invert ? ~mClutMap[c % 256] : mClutMap[c % 256]);
            }
        }
        EPD_Display_end();
        break;
    case DATATYPE_IMG_BMP:;
        printf ("sending BMP to EPD - ");

        printf (" complete.\r\n");
        break;
    default:  // prevent drawing from an unknown file image type
        printf ("Image with type 0x%02X was requested, but we don't know what to do with that currently...\r\n", eih->dataType);
        return;
    }
}
