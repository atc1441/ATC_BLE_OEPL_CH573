#include "main.h"
#include "CH57x_common.h"
#include "config.h"
#include "HAL.h"
#include "peripheral.h"
#include "epd.h"
#include "proto.h"
#include "tl_common.h"
#include "app_bleCmdHandle.h"

extern uint32_t doRebootOrFirmwareUpdate;
static void handleBlkBLE (u8 len, u8 *payload);

int zb_ble_hci_cmd_handler (u16 cmdId, u8 len, u8 *payload) {
    int ret = 0;
    if (len < 2) {
        printf ("To little data 2 byte CMD minimum\r\n", len);
        return 0;
    }
    len -= 2;
    printf ("Received %u of BLE Data\r\n", len);
    switch (cmdId) {
    default:
        ble_send_notify (BLE_CMD_ERR, NULL, 0);
        break;
    case BLE_CMD_FAST:
        ble_set_connection_speed (8);
        ble_send_notify (BLE_CMD_ACK_CMD, NULL, 0);
        break;
    case BLE_CMD_SLOW:
        ble_set_connection_speed (400);
        ble_send_notify (BLE_CMD_ACK_CMD, NULL, 0);
        break;
    case BLE_CMD_AVAILDATA:
        processAvailDataInfoBLE ((struct AvailDataInfo *)payload);
        break;
    case BLE_CMD_BLK_DATA:
        // ble_set_connection_speed (8);
        handleBlkBLE (len, payload);
        break;
    case BLE_CMD_LED:
        // set_led_color(payload[0]);
        break;

    case BLE_CMD_GET_INFOS:
        ble_send_notify (BLE_CMD_GET_INFOS, (uint8_t *)&settings, sizeof (settings_struct));
        break;
    }
    return ret;
}

#define BLOCK_PART_DATA_SIZE_BLE 230
static uint16_t dataRequestSizeBLE = 0;
static uint8_t partsThisBlockBLE = 0;
static uint8_t blockXferBuffer[BLOCK_XFER_BUFFER_SIZE];

struct blockRequest curBlock;  // used by the block-requester, contains the next request that we'll send
struct AvailDataInfo curDataInfo;
uint8_t curImgSlot;
uint32_t curHighSlotId;
uint8_t nextImgSlot;
uint8_t imgSlots;
uint8_t drawWithLut;
uint32_t imageSize;

uint32_t getAddressForSlot (const uint8_t s) {
    return EEPROM_IMG_START + (settings.saved_EEPROM_IMG_EACH * s);
}

void drawImageFromEeprom (const uint8_t imgSlot) {
    drawImageAtAddress (getAddressForSlot (imgSlot), drawWithLut);
    drawWithLut = 0;  // default back to the regular ol' stock/OTP LUT
}

void saveImgBlockData (const uint8_t imgSlot, const uint8_t blockId) {
    uint32_t length = settings.saved_EEPROM_IMG_EACH - (sizeof (struct EepromImageHeader) + (blockId * BLOCK_DATA_SIZE));
    if (length > 4096)
        length = 4096;
    if (!eepromWrite (getAddressForSlot (imgSlot) + sizeof (struct EepromImageHeader) + (blockId * BLOCK_DATA_SIZE), blockXferBuffer + sizeof (struct blockData), length))
        printf ("EEPROM write failed\r\n");
}

uint8_t findSlot (const uint8_t *ver) {
    // return 0xFF; // remove me! This forces the tag to re-download each and every upload without checking if it's already in the eeprom somewhere
    uint32_t markerValid = EEPROM_IMG_VALID + settings.saved_EEPROM_IMG_EACH;
    for (uint8_t c = 0; c < imgSlots; c++) {
        struct EepromImageHeader *eih = (struct EepromImageHeader *)blockXferBuffer;
        eepromRead (getAddressForSlot (c), (uint8_t *)eih, sizeof (struct EepromImageHeader));
        if (!memcmp (&eih->validMarker, &markerValid, 4)) {
            if (!memcmp (&eih->version, (void *)ver, 8)) {
                return c;
            }
        }
    }
    return 0xFF;
}

uint8_t checkCRC (const void *p, const uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t *)p)[c];
    }
    // printf("CRC: rx %d, calc %d\r\n", ((uint8_t *)p)[0], total);
    return ((uint8_t *)p)[0] == total;
}

uint8_t validateBlockData() {
    struct blockData *bd = (struct blockData *)blockXferBuffer;
    printf ("expected len = %04X, checksum=%04X\r\n", bd->size, bd->checksum);
    if (bd->size > BLOCK_XFER_BUFFER_SIZE - sizeof (struct blockData)) {
        printf ("Impossible data size, we abort here\r\n");
        return 0;
    }
    uint16_t t = 0;
    for (uint16_t c = 0; c < bd->size; c++) {
        t += bd->data[c];
    }
    printf ("Checked len = %04X, checksum=%04X\r\n", bd->size, t);
    return bd->checksum == t;
}

static uint8_t processBlockPartBLE (const struct blockPart *bp) {
    uint16_t start = bp->blockPart * BLOCK_PART_DATA_SIZE_BLE;
    uint16_t size = BLOCK_PART_DATA_SIZE_BLE;
    // validate if it's okay to copy data
    if (bp->blockId != curBlock.blockId) {
        printf ("got a packet for block %02X %02X\r\n", bp->blockId, curBlock.blockId);
        return 0;
    }
    if (start >= (sizeof (blockXferBuffer) - 1)) {
        printf ("Too big %u\r\n", start);
        return 0;
    }
    if (bp->blockPart > BLOCK_MAX_PARTS) {
        printf ("Block too high  %u\r\n", bp->blockPart);
        return 0;
    }
    if ((start + size) > sizeof (blockXferBuffer)) {
        size = sizeof (blockXferBuffer) - start;
    }
    if (checkCRC (bp, sizeof (struct blockPart) + BLOCK_PART_DATA_SIZE_BLE)) {
        //  copy block data to buffer
        memcpy ((void *)(blockXferBuffer + start), (const void *)bp->data, size);
        // we don't need this block anymore, set bit to 0 so we don't request it again
        curBlock.requestedParts[bp->blockPart / 8] &= ~(1 << (bp->blockPart % 8));
        return 1;
    } else {
        printf ("CRC Failed \r\n");
        return 0;
    }
}

uint8_t data_Type_download = 0;

uint8_t processAvailDataInfoBLE (struct AvailDataInfo *avail) {
    printf ("dataType: %d\r\n", avail->dataType);
    switch (avail->dataType) {
    case DATATYPE_IMG_BMP:
    case DATATYPE_IMG_DIFF:
        break;
    case DATATYPE_IMG_ZLIB:
        printf ("Lets NOT download a compressed image\r\n");
        break;
    case DATATYPE_IMG_RAW_1BPP:
    case DATATYPE_IMG_RAW_2BPP:
        printf ("RAW_BPP\r\n");
        if (avail->dataSize + sizeof (struct EepromImageHeader) >= settings.saved_EEPROM_IMG_EACH) {
            printf ("Image is too big! %08X > %08X\r\n", avail->dataSize, settings.saved_EEPROM_IMG_EACH);
            ble_send_notify (BLE_CMD_ERR, NULL, 0);
            return 1;
        }
        // check if this download is currently displayed or active
        if (curDataInfo.dataSize == 0 && !memcmp ((const void *)&avail->dataVer, (const void *)&curDataInfo.dataVer, 8)) {
            // we've downloaded this already, we're guessing it's already displayed
            printf ("currently shown image, send xfc\r\n");
            ble_send_notify (BLE_CMD_ACK_IS_SHOWN, NULL, 0);
            return 1;
        }
        // check if we've seen this version before
        curImgSlot = findSlot ((uint8_t *)&(avail->dataVer));
        if (curImgSlot != 0xFF) {
            // found a (complete)valid image slot for this version
            ble_send_notify (BLE_CMD_ACK, NULL, 0);

            printf ("already seen, drawing from eeprom slot %d\r\n", curImgSlot);

            // mark as completed and draw from EEPROM
            memcpy (&curDataInfo, (void *)avail, sizeof (struct AvailDataInfo));
            curDataInfo.dataSize = 0;  // mark as transfer not pending

            drawWithLut = avail->dataTypeArgument;
            // wdt60s();
            // drawOnOffline (1);
            drawImageFromEeprom (curImgSlot);
            return 1;
        } else {
            nextImgSlot++;
            if (nextImgSlot >= imgSlots)
                nextImgSlot = 0;
            curImgSlot = nextImgSlot;
            printf ("Saving to image slot %d\r\n", curImgSlot);
            drawWithLut = avail->dataTypeArgument;
            uint8_t attempt = 5;
            while (attempt--) {
                if (eepromErase (getAddressForSlot (curImgSlot), settings.saved_EEPROM_IMG_EACH))
                    goto eraseSuccess;
            }
            // eepromFail:
            printf ("EEPROM ERROR\r\n");
eraseSuccess:
            printf ("new download, writing to slot %d\r\n", curImgSlot);
            // start, or restart the transfer. Copy data from the AvailDataInfo struct, and the struct intself. This forces a new transfer
            curBlock.blockId = 0;
            memcpy (&(curBlock.ver), &(avail->dataVer), 8);
            curBlock.type = avail->dataType;
            memcpy (&curDataInfo, (void *)avail, sizeof (struct AvailDataInfo));
            imageSize = curDataInfo.dataSize;
            memset (curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
            if (curDataInfo.dataSize > BLOCK_DATA_SIZE) {
                // more than one block remaining
                dataRequestSizeBLE = BLOCK_DATA_SIZE;
            } else {
                // only one block remains
                dataRequestSizeBLE = curDataInfo.dataSize;
            }
            partsThisBlockBLE = (sizeof (struct blockData) + dataRequestSizeBLE) / BLOCK_PART_DATA_SIZE_BLE;
            if ((sizeof (struct blockData) + dataRequestSizeBLE) % BLOCK_PART_DATA_SIZE_BLE)
                partsThisBlockBLE++;
            printf ("Request next block! %u %08X \r\n", partsThisBlockBLE, curDataInfo.dataSize);
            ble_send_notify (BLE_CMD_REQ, (uint8_t *)&curBlock, sizeof (struct blockRequest));
        } /**/
        break;
    case DATATYPE_FW_UPDATE:
        printf ("Firmware\r\n");
        break;
    case DATATYPE_NFC_URL_DIRECT:
    case DATATYPE_NFC_RAW_CONTENT:
        break;
    case DATATYPE_CUSTOM_LUT_OTA:
        return 1;
        break;
    case DATATYPE_COMMAND_DATA: {
        uint8_t dataArgument = avail->dataTypeArgument;
        printf ("Received a CMD: %02X\r\n", dataArgument);
    }
    }
    return 1;
}

static void handleBlkBLE (u8 len, u8 *payload) {
    if (processBlockPartBLE ((struct blockPart *)payload)) {
        printf ("RX  %d[", curBlock.blockId);
        for (uint8_t c = 0; c < partsThisBlockBLE; c++) {
            if ((c != 0) && (c % 8 == 0))
                printf ("][");
            if (curBlock.requestedParts[c / 8] & (1 << (c % 8))) {
                printf (".");
            } else {
                printf ("R");
            }
        }
        printf ("]\r\n");

        uint8_t blockComplete = 1;
        for (uint8_t c = 0; c < partsThisBlockBLE; c++) {
            if (curBlock.requestedParts[c / 8] & (1 << (c % 8)))
                blockComplete = 0;
        }

        if (blockComplete) {
            printf ("- COMPLETE\r\n");
            if (validateBlockData()) {
                printf ("- Validated\r\n");
                printf ("Saving block %d to slot %d\r\n", curBlock.blockId, curImgSlot);
                saveImgBlockData (curImgSlot, curBlock.blockId);
                curBlock.blockId++;
                curDataInfo.dataSize -= dataRequestSizeBLE;
                if (!curDataInfo.dataSize) {
                    ble_set_connection_speed (400);
                    struct EepromImageHeader *eih = (struct EepromImageHeader *)blockXferBuffer;
                    memcpy (&eih->version, &curDataInfo.dataVer, 8);
                    eih->validMarker = EEPROM_IMG_VALID + settings.saved_EEPROM_IMG_EACH;
                    eih->id = ++curHighSlotId;
                    eih->size = imageSize;
                    eih->dataType = curDataInfo.dataType;
                    printf ("Now writing datatype 0x%02X to slot %d\r\n", curDataInfo.dataType, curImgSlot);
                    eepromWrite (getAddressForSlot (curImgSlot), (uint8_t *)eih, sizeof (struct EepromImageHeader));
                    printf ("download complete!\r\n");
                    ble_send_notify (BLE_CMD_ACK, NULL, 0);
                    // wdt60s();
                    drawOnOffline (1);
                    drawImageFromEeprom (curImgSlot);

                } else {
                    memset (curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
                    if (curDataInfo.dataSize > BLOCK_DATA_SIZE) {
                        // more than one block remaining
                        dataRequestSizeBLE = BLOCK_DATA_SIZE;
                    } else {
                        // only one block remains
                        dataRequestSizeBLE = curDataInfo.dataSize;
                    }
                    partsThisBlockBLE = (sizeof (struct blockData) + dataRequestSizeBLE) / BLOCK_PART_DATA_SIZE_BLE;
                    if ((sizeof (struct blockData) + dataRequestSizeBLE) % BLOCK_PART_DATA_SIZE_BLE)
                        partsThisBlockBLE++;
                    printf ("Request next block! %u %08X \r\n", partsThisBlockBLE, curDataInfo.dataSize);
                    ble_send_notify (BLE_CMD_REQ, (uint8_t *)&curBlock, sizeof (struct blockRequest));
                }
            } else {
                memset (curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
                printf ("blk failed validation!\r\n");
                if (curDataInfo.dataSize > BLOCK_DATA_SIZE) {
                    // more than one block remaining
                    dataRequestSizeBLE = BLOCK_DATA_SIZE;
                } else {
                    // only one block remains
                    dataRequestSizeBLE = curDataInfo.dataSize;
                }
                partsThisBlockBLE = (sizeof (struct blockData) + dataRequestSizeBLE) / BLOCK_PART_DATA_SIZE_BLE;
                if ((sizeof (struct blockData) + dataRequestSizeBLE) % BLOCK_PART_DATA_SIZE_BLE)
                    partsThisBlockBLE++;
                printf ("Request next block! %u %08X \r\n", partsThisBlockBLE, curDataInfo.dataSize);
                ble_send_notify (BLE_CMD_REQ, (uint8_t *)&curBlock, sizeof (struct blockRequest));
            }
        } else {
            ble_send_notify (BLE_CMD_ACK_BLKPRT, NULL, 0);
        }
    } else {
        ble_send_notify (BLE_CMD_ERR_BLKPRT, NULL, 0);
        printf ("Something is wrong with the packet!\r\n");
        for (int i = 0; i < len; i++) {
            printf (" %02X", payload[i]);
        }
        printf ("\r\n");
    }
}
