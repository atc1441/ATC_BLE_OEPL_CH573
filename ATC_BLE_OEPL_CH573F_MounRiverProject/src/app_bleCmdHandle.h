#pragma once
#include "main.h"
#include "proto.h"
#include "CH57x_common.h"
#include "config.h"
#include "HAL.h"
#include "peripheral.h"
#include "epd.h"

#define BLE_CMD_LED 1
#define BLE_CMD_FAST 2
#define BLE_CMD_SLOW 3
#define BLE_CMD_SET_TYPE 4
#define BLE_CMD_GET_INFOS 5
#define BLE_CMD_OEPL_ENABLE 6
#define BLE_CMD_OEPL_DISABLE 7
#define BLE_CMD_OEPL_BLE_PERIOD 8
#define BLE_CMD_SET_CUSTOM_MAC 9
#define BLE_CMD_RESET_TO_DEFAULT 10


#define BLE_CMD_ACK_CMD 99
#define BLE_CMD_AVAILDATA 100
#define BLE_CMD_BLK_DATA 101
#define BLE_CMD_ERR_BLKPRT 196
#define BLE_CMD_ACK_BLKPRT 197
#define BLE_CMD_REQ 198
#define BLE_CMD_ACK 199
#define BLE_CMD_ACK_IS_SHOWN 200
#define BLE_CMD_ACK_FW_UPDATED 201

#define BLE_CMD_ERR 0xffff


int zb_ble_hci_cmd_handler(u16 cmdId, u8 len, u8 *payload);
uint8_t processAvailDataInfoBLE(struct AvailDataInfo *avail);
