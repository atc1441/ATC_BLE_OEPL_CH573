#include "epd.h"
#include "CH57x_common.h"
#include "config.h"
#include "HAL.h"
#include "main.h"
#include "tl_common.h"

typedef uint8_t (*EPD_start_Function) (uint8_t);
typedef void (*EPD_send_byte_Function) (uint8_t);
typedef void (*EPD_color_change_Function) (uint8_t);
typedef uint8_t (*EPD_busy_Function) (void);
typedef void (*EPD_end_Function) (void);
typedef void (*EPD_sleep_Function) (void);
#define SUPPORTED_FUNCTION_MAX 3
EPD_start_Function EPD_start_arr[SUPPORTED_FUNCTION_MAX] = {NULL, EPD_UC_Display_start, EPD_SSD_Display_start};
EPD_send_byte_Function EPD_send_byte_arr[SUPPORTED_FUNCTION_MAX] = {NULL, EPD_UC_Display_byte, EPD_SSD_Display_byte};
EPD_color_change_Function EPD_color_change_arr[SUPPORTED_FUNCTION_MAX] = {NULL, EPD_UC_Display_color_change, EPD_SSD_Display_color_change};
EPD_busy_Function EPD_busy_arr[SUPPORTED_FUNCTION_MAX] = {NULL, EPD_UC_busy_check, EPD_SSD_busy_check};
EPD_end_Function EPD_end_arr[SUPPORTED_FUNCTION_MAX] = {NULL, EPD_UC_Display_end, EPD_SSD_Display_end};
EPD_sleep_Function EPD_sleep_arr[SUPPORTED_FUNCTION_MAX] = {NULL, EPD_UC_set_sleep, EPD_SSD_set_sleep};


uint8_t epd_update_state = 0;

void EPD_Init() {
    GPIOA_ModeCfg (EPD_DC, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg (EPD_RESET, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg (EPD_BUSY, GPIO_ModeIN_Floating);
    GPIOA_ModeCfg (EPD_MOSI, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg (EPD_CLK, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg (EPD_CS, GPIO_ModeOut_PP_5mA);

    GPIOA_SetBits (EPD_CS);

    EPD_Display_start (1);

    for (uint32_t Y = 0; Y < settings.screen_h; Y++) {
        for (uint32_t X = 0; X < (settings.screen_w / 8); X++) {
            EPD_Display_byte (settings.screen_color_black_invert ? ~0x00 : 0x00);
        }
    }

    EPD_Display_color_change (1);
    for (uint32_t Y = 0; Y < settings.screen_h; Y++) {
        for (uint32_t X = 0; X < (settings.screen_w / 8); X++) {
            EPD_Display_byte (settings.screen_color_second_invert ? ~0x00 : 0x00);
        }
    }
    EPD_Display_end();
}

void EPD_Display_start (uint8_t full_or_partial) {
    printf ("EPD_Display_start\r\n");
    if (settings.screen_functions < SUPPORTED_FUNCTION_MAX && EPD_start_arr[settings.screen_functions]) {
        DelayMs (5);
        GPIOA_ResetBits (EPD_RESET);
        DelayMs (20);
        GPIOA_SetBits (EPD_RESET);
        DelayMs (20);
        EPD_start_arr[settings.screen_functions](1);
        epd_update_state = 1;
    }
}

void EPD_Display_byte (uint8_t data) {
    if (settings.screen_functions < SUPPORTED_FUNCTION_MAX && EPD_send_byte_arr[settings.screen_functions]) {
        EPD_send_byte_arr[settings.screen_functions](data);
    }
}

void EPD_Display_color_change (uint8_t newColor) {
    printf ("EPD_Display_color_change\r\n");
    if (settings.screen_functions < SUPPORTED_FUNCTION_MAX && EPD_color_change_arr[settings.screen_functions]) {
        EPD_color_change_arr[settings.screen_functions](newColor);
    }
}

void EPD_Display_end() {
    printf ("EPD_Display_end\r\n");
    if (settings.screen_functions < SUPPORTED_FUNCTION_MAX && EPD_end_arr[settings.screen_functions]) {
        EPD_end_arr[settings.screen_functions]();
        epd_update_state = 2;
        // cpu_set_gpio_wakeup(settings.epd_pinout->BUSY, !drv_gpio_read(settings.epd_pinout->BUSY), 1);
    }
}

void epd_set_sleep (void) {
    printf ("epd_set_sleep\r\n");
    if (settings.screen_functions < SUPPORTED_FUNCTION_MAX && EPD_sleep_arr[settings.screen_functions]) {
        EPD_sleep_arr[settings.screen_functions]();
        // EPD_POWER_OFF();
        // EPD_GPIO_end();
        epd_update_state = 0;
    }
}

uint8_t epd_state_handler (void) {
    if (settings.screen_functions < SUPPORTED_FUNCTION_MAX && EPD_busy_arr[settings.screen_functions]) {
        switch (epd_update_state) {
        case 0:
            // Nothing to do
            break;
        case 1:
            // Nothing to do
            break;
        case 2:  // check if refresh is done and sleep epd if so
            if (EPD_busy_arr[settings.screen_functions]()) {
                epd_set_sleep();
            }
            break;
        }
        return epd_update_state;
    }
    return 0;
}

void EPD_CheckStatus (int max_ms) {
    DelayMs (1);
    while (!GPIOA_ReadPortPin (EPD_BUSY)) {
    }
}

void EPD_CheckStatus_inverted (int max_ms) {
    DelayMs (1);
    while (GPIOA_ReadPortPin (EPD_BUSY)) {
    }
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