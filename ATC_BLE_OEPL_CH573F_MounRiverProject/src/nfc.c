#include "nfc.h"
#include "CH57x_common.h"
#include "config.h"
#include "HAL.h"
#include "main.h"

void NFC_Init() {
    GPIOA_ModeCfg (NFC_CS, GPIO_ModeIN_PU);
    GPIOA_ModeCfg (NFC_CLK, GPIO_ModeIN_Floating);
    GPIOB_ModeCfg (NFC_IRQ, GPIO_ModeIN_Floating);
    GPIOB_ModeCfg (NFC_MOSI, GPIO_ModeIN_Floating);
}
