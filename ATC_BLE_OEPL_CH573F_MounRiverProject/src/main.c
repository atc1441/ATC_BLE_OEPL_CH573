
#include "main.h"
#include "CH57x_common.h"
#include "config.h"
#include "HAL.h"
#include "peripheral.h"
#include "tl_common.h"


__attribute__ ((aligned (4))) uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];

#if (defined(BLE_MAC)) && (BLE_MAC == TRUE)
const uint8_t MacAddr[6] =
    {0x84, 0xC2, 0xE4, 0x03, 0x02, 0x02};
#endif

__attribute__ ((section (".highcode"))) void Main_Circulation() {
    reset_settings_to_default (DEFAULT_DEVICE_TYPE);
    GPIOB_ModeCfg (LED_R, GPIO_ModeIN_Floating);
    GPIOB_ModeCfg (LED_G, GPIO_ModeIN_Floating);
    GPIOB_ModeCfg (LED_B, GPIO_ModeIN_Floating);
    NFC_Init();
    FLASH_Init();
    EPD_Init();
    while (1) {
        epd_state_handler();
        TMOS_SystemProcess();
    }
}

int main() {
#if (defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
    PWR_DCDCCfg (ENABLE);
#endif
    SetSysClock (CLK_SOURCE_PLL_60MHz);
#if (defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
    GPIOA_ModeCfg (GPIO_Pin_All, GPIO_ModeIN_PU);
    GPIOB_ModeCfg (GPIO_Pin_All, GPIO_ModeIN_PU);
#endif
#ifdef DEBUG
    GPIOA_SetBits (bTXD1);
    GPIOA_ModeCfg (bTXD1, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
#endif
    PRINT ("%s\n", VER_LIB);
    CH57X_BLEInit();
    HAL_Init();
    GAPRole_PeripheralInit();
    Peripheral_Init();
    Main_Circulation();
}
