#pragma once

#define FIRMWARE_VERSION 1
#define HW_TYPE 0x02


// Port A
// VERSION 2.2 PCB!!!
#define EPD_DC GPIO_Pin_4
#define EPD_RESET GPIO_Pin_5
#define EPD_BUSY GPIO_Pin_15
#define EPD_MOSI GPIO_Pin_14
#define EPD_CLK GPIO_Pin_13
#define EPD_CS GPIO_Pin_12


// VERSION 1.0 PCB!!!
/*#define EPD_DC GPIO_Pin_15
#define EPD_RESET GPIO_Pin_5
#define EPD_BUSY GPIO_Pin_4
#define EPD_MOSI GPIO_Pin_12
#define EPD_CLK GPIO_Pin_13
#define EPD_CS GPIO_Pin_14*/

// Port A
#define NFC_CS GPIO_Pin_10
#define NFC_CLK GPIO_Pin_11
// Port B
#define NFC_IRQ GPIO_Pin_12
#define NFC_MOSI GPIO_Pin_13

// Port B
#define SPI_FLASH_CS GPIO_Pin_4
#define SPI_FLASH_CLK GPIO_Pin_7
#define SPI_FLASH_MISO GPIO_Pin_10
#define SPI_FLASH_MOSI GPIO_Pin_11

// Port B
#define LED_R GPIO_Pin_22
#define LED_G GPIO_Pin_14
#define LED_B GPIO_Pin_15
