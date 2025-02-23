#include "main.h"
#include "CH57x_common.h"
#include "config.h"
#include "HAL.h"
#include "peripheral.h"
#include "epd.h"
#include "proto.h"
#include "app_bleCmdHandle.h"

extern uint32_t doRebootOrFirmwareUpdate;
static void handleBlkBLE(u8 len, u8 *payload);

int zb_ble_hci_cmd_handler(u16 cmdId, u8 len, u8 *payload){
	int ret = 0;
	if(len<2)
		{
		printf("To little data 2 byte CMD minimum\r\n", len);
		return 0;
		}
	len -= 2;
	printf("Received %u of BLE Data\r\n", len);
	switch(cmdId)
	{
	default:
    	ble_send_notify(BLE_CMD_ERR, NULL,0);
		break;
	case BLE_CMD_FAST:
		ble_set_connection_speed(8);
    	ble_send_notify(BLE_CMD_ACK_CMD, NULL,0);
		break;
	case BLE_CMD_SLOW:
		ble_set_connection_speed(400);
    	ble_send_notify(BLE_CMD_ACK_CMD, NULL,0);
		break;
	case BLE_CMD_AVAILDATA:
		processAvailDataInfoBLE((struct AvailDataInfo *)payload);
		break;
	case BLE_CMD_BLK_DATA:
		ble_set_connection_speed(8);
		handleBlkBLE(len, payload);
		break;
	/*case BLE_CMD_LED:
		set_led_color(payload[0]);
		break;
	case BLE_CMD_SET_TYPE:
		if(len == 2){
	    	ble_send_notify(BLE_CMD_ACK_CMD, NULL,NULL);
	    	settings_set_screen_data((payload[0]<<8)|payload[1]);
			save_settings_to_flash();
			WaitMs(100);
			doRebootOrFirmwareUpdate = 0x84722338;
		}else{
	    	ble_send_notify(BLE_CMD_ERR, NULL,NULL);
		}
		break;
	case BLE_CMD_GET_INFOS:
    	ble_send_notify(BLE_CMD_GET_INFOS, (uint8_t *)&settings, sizeof(settings_struct));
		break;
	case BLE_CMD_OEPL_ENABLE:
		settings.oepl_enabled = true;
		printf("OEPL is now %s\r\n", settings.oepl_enabled?"enabled":"disabled");
    	ble_send_notify(BLE_CMD_ACK_CMD, NULL,NULL);
		break;
	case BLE_CMD_OEPL_DISABLE:
		settings.oepl_enabled = false;
		printf("OEPL is now %s\r\n", settings.oepl_enabled?"enabled":"disabled");
    	ble_send_notify(BLE_CMD_ACK_CMD, NULL,NULL);
		break;
	case BLE_CMD_OEPL_BLE_PERIOD:
	{
		uint16_t newAdvInterval = (payload[0]<<8)|payload[1];
		if(newAdvInterval < 100 || newAdvInterval > 10000)
			newAdvInterval = 3200;
		printf("Setting new ADV interval: %u\r\n", newAdvInterval);
		settings.ble_wakeup_time = newAdvInterval;
    	ble_send_notify(BLE_CMD_ACK_CMD, NULL,NULL);
	}
		break;
	case BLE_CMD_SET_CUSTOM_MAC:
		if(len == 8){
	    	ble_send_notify(BLE_CMD_ACK_CMD, NULL,NULL);
	    	set_custom_mac(&payload[0]);
			save_settings_to_flash();
			WaitMs(100);
			doRebootOrFirmwareUpdate = 0x84722338;
		}else{
	    	ble_send_notify(BLE_CMD_ERR, NULL,NULL);
		}
		break;
	case BLE_CMD_RESET_TO_DEFAULT:
		if(len == 2 && payload[0] == 0x12 && payload[1] == 0x34){
	    	ble_send_notify(BLE_CMD_ACK_CMD, NULL,NULL);
	    	reset_settings_to_default(settings.screen_type);
			save_settings_to_flash();
			WaitMs(100);
			doRebootOrFirmwareUpdate = 0x84722338;
		}else{
	    	ble_send_notify(BLE_CMD_ERR, NULL,NULL);
		}
		break;*/
	}
	return ret;
}

#define BLOCK_PART_DATA_SIZE_BLE 230
static uint16_t dataRequestSizeBLE = 0;
static uint8_t partsThisBlockBLE = 0;
/*extern uint8_t blockXferBuffer[BLOCK_XFER_BUFFER_SIZE];
extern struct blockRequest curBlock;     // used by the block-requester, contains the next request that we'll send
extern struct AvailDataInfo curDataInfo;
extern uint8_t curImgSlot;
extern uint32_t curHighSlotId;
extern uint8_t nextImgSlot;
extern uint8_t imgSlots;
extern uint8_t drawWithLut;
extern uint32_t imageSize;
extern uint8_t findSlot(const uint8_t *ver);
extern uint32_t getAddressForSlot(const uint8_t s);
extern uint8_t checkCRC(const void *p, const uint8_t len);
extern uint8_t validateBlockData();
extern void saveImgBlockData(const uint8_t imgSlot, const uint8_t blockId);
extern void eraseUpdateBlock();
extern void saveUpdateBlockData(uint8_t blockId);*/

static uint8_t processBlockPartBLE(const struct blockPart *bp)
{
    /*uint16_t start = bp->blockPart * BLOCK_PART_DATA_SIZE_BLE;
    uint16_t size = BLOCK_PART_DATA_SIZE_BLE;
    // validate if it's okay to copy data
    if (bp->blockId != curBlock.blockId)
    {
        printf("got a packet for block %02X %02X\r\n", bp->blockId, curBlock.blockId);
        return false;
    }
    if (start >= (sizeof(blockXferBuffer) - 1))
        {
        printf("Too big %u\r\n", start);
    	return false;
        }
    if (bp->blockPart > BLOCK_MAX_PARTS)
        {
        printf("Block too high  %u\r\n", bp->blockPart);
    	return false;
        }
    if ((start + size) > sizeof(blockXferBuffer))
    {
        size = sizeof(blockXferBuffer) - start;
    }
    if (checkCRC(bp, sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE_BLE))
    {
        //  copy block data to buffer
        memcpy((void *)(blockXferBuffer + start), (const void *)bp->data, size);
        // we don't need this block anymore, set bit to 0 so we don't request it again
        curBlock.requestedParts[bp->blockPart / 8] &= ~(1 << (bp->blockPart % 8));
        return true;
    }
    else*/
    {
        printf("CRC Failed \r\n");
        return 0;
    }
}

uint8_t data_Type_download = 0;
uint8_t processAvailDataInfoBLE(struct AvailDataInfo *avail)
{
    printf("dataType: %d\r\n", avail->dataType);
    /*switch (avail->dataType)
    {
    case DATATYPE_IMG_BMP:
    case DATATYPE_IMG_DIFF:
        break;
    case DATATYPE_IMG_ZLIB:
        printf("Lets download a compressed image\r\n");
    case DATATYPE_IMG_RAW_1BPP:
    case DATATYPE_IMG_RAW_2BPP:
        printf("RAW_BPP\r\n");
        if(avail->dataSize + sizeof(struct EepromImageHeader) >= settings.saved_EEPROM_IMG_EACH)
        {
            printf("Image is too big! %08X > %08X\r\n", avail->dataSize, settings.saved_EEPROM_IMG_EACH);
        	ble_send_notify(BLE_CMD_ERR, NULL,NULL);
        	return true;
        }
        // check if this download is currently displayed or active
        if (curDataInfo.dataSize == 0 && !memcmp((const void *)&avail->dataVer, (const void *)&curDataInfo.dataVer, 8))
        {
            // we've downloaded this already, we're guessing it's already displayed
            printf("currently shown image, send xfc\r\n");
        	ble_send_notify(BLE_CMD_ACK_IS_SHOWN, NULL,NULL);
            return true;
        }
        // check if we've seen this version before
        curImgSlot = findSlot((uint8_t *)&(avail->dataVer));
        if (curImgSlot != 0xFF)
        {
            // found a (complete)valid image slot for this version
        	ble_send_notify(BLE_CMD_ACK, NULL,NULL);

            printf("already seen, drawing from eeprom slot %d\r\n", curImgSlot);

            // mark as completed and draw from EEPROM
            memcpy(&curDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
            curDataInfo.dataSize = 0; // mark as transfer not pending

            drawWithLut = avail->dataTypeArgument;
            wdt60s();
            drawOnOffline(1);
            drawImageFromEeprom(curImgSlot);
            return true;
        }
        else
        {
            nextImgSlot++;
            if (nextImgSlot >= imgSlots)
                nextImgSlot = 0;
            curImgSlot = nextImgSlot;
            printf("Saving to image slot %d\r\n", curImgSlot);
            drawWithLut = avail->dataTypeArgument;
            uint8_t attempt = 5;
            while (attempt--)
            {
                if (eepromErase(getAddressForSlot(curImgSlot), settings.saved_EEPROM_IMG_EACH))
                    goto eraseSuccess;
            }
            // eepromFail:
            printf("EEPROM ERROR\r\n");
        eraseSuccess:
            printf("new download, writing to slot %d\r\n", curImgSlot);
            // start, or restart the transfer. Copy data from the AvailDataInfo struct, and the struct intself. This forces a new transfer
            curBlock.blockId = 0;
            memcpy(&(curBlock.ver), &(avail->dataVer), 8);
            curBlock.type = avail->dataType;
            memcpy(&curDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
            imageSize = curDataInfo.dataSize;
            memset(curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
            if (curDataInfo.dataSize > BLOCK_DATA_SIZE)
            {
                // more than one block remaining
            	dataRequestSizeBLE = BLOCK_DATA_SIZE;
            }
            else
            {
                // only one block remains
            	dataRequestSizeBLE = curDataInfo.dataSize;
            }
            	partsThisBlockBLE = (sizeof(struct blockData) + dataRequestSizeBLE) / BLOCK_PART_DATA_SIZE_BLE;
                if ((sizeof(struct blockData) + dataRequestSizeBLE) % BLOCK_PART_DATA_SIZE_BLE)
                	partsThisBlockBLE++;
            printf("Request next block! %u %08X \r\n", partsThisBlockBLE, curDataInfo.dataSize);
        	ble_send_notify(BLE_CMD_REQ, (uint8_t *)&curBlock,sizeof(struct blockRequest));
        }
        break;
    case DATATYPE_FW_UPDATE:
        printf("Firmware\r\n");
        if(avail->dataSize > EEPROM_UPDATE_AREA_LEN)
        {
            printf("Update is too big! %08X > %08X\r\n", avail->dataSize, EEPROM_UPDATE_AREA_LEN);
        	ble_send_notify(BLE_CMD_ERR, NULL,NULL);
        	return true;
        }
        // check if we already started the transfer of this information & haven't completed it
        if (curDataInfo.dataSize && !memcmp((const void *)&avail->dataVer, (const void *)&curDataInfo.dataVer, 8))
        {
            // looks like we did. We'll carry on where we left off.
        }
        else
        {
            // start, or restart the transfer from 0. Copy data from the AvailDataInfo struct, and the struct intself. This forces a new transfer
            curBlock.blockId = 0;
            memcpy(&(curBlock.ver), &(avail->dataVer), 8);
            curBlock.type = avail->dataType;
            memcpy(&curDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
            eraseUpdateBlock();
        }

        {
            memset(curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
            if (curDataInfo.dataSize > BLOCK_DATA_SIZE)
            {
                // more than one block remaining
            	dataRequestSizeBLE = BLOCK_DATA_SIZE;
            }
            else
            {
                // only one block remains
            	dataRequestSizeBLE = curDataInfo.dataSize;
            }
            	partsThisBlockBLE = (sizeof(struct blockData) + dataRequestSizeBLE) / BLOCK_PART_DATA_SIZE_BLE;
                if ((sizeof(struct blockData) + dataRequestSizeBLE) % BLOCK_PART_DATA_SIZE_BLE)
                	partsThisBlockBLE++;
            printf("Request next block! %u %08X \r\n", partsThisBlockBLE, curDataInfo.dataSize);
        	ble_send_notify(BLE_CMD_REQ, (uint8_t *)&curBlock,sizeof(struct blockRequest));
        }
        break;
    case DATATYPE_NFC_URL_DIRECT:
    case DATATYPE_NFC_RAW_CONTENT:
        break;
    case DATATYPE_CUSTOM_LUT_OTA:
        return true;
        break;
    case DATATYPE_COMMAND_DATA:
    {
    	uint8_t dataArgument = avail->dataTypeArgument;
        printf("Received a CMD: %02X\r\n", dataArgument);
        if(dataArgument >= 0xA0 && (dataArgument - 0xA0) < MAX_LEN_TYPE){
        	ble_send_notify(BLE_CMD_ACK, NULL,NULL);
            	settings_set_screen_data(dataArgument - 0xA0);
        		save_settings_to_flash();
        		WaitMs(100);
        		doRebootOrFirmwareUpdate = 0x84722338;// New settings will be aplied on reboot
        }else{
            switch (dataArgument)
            {
            case CMD_DO_REBOOT:
            {
            	ble_send_notify(BLE_CMD_ACK, NULL,NULL);
    			WaitMs(100);
    			doRebootOrFirmwareUpdate = 0x84722338;
            }
            break;
        }
        }
    }
    }*/
    return 1;
}

static void handleBlkBLE(u8 len, u8 *payload)
{
	/*if(processBlockPartBLE((struct blockPart*)payload))
	{
        printf("RX  %d[", curBlock.blockId);
        for (uint8_t c = 0; c < partsThisBlockBLE; c++)
        {
            if ((c != 0) && (c % 8 == 0))
                printf("][");
            if (curBlock.requestedParts[c / 8] & (1 << (c % 8)))
            {
                printf(".");
            }
            else
            {
                printf("R");
            }
        }
        printf("]\r\n");
		   bool blockComplete = true;
		        for (uint8_t c = 0; c < partsThisBlockBLE; c++)
		        {
		            if (curBlock.requestedParts[c / 8] & (1 << (c % 8)))
		                blockComplete = false;
		        }

		        if (blockComplete)
		        {
		            printf("- COMPLETE\r\n");
		            if (validateBlockData())
		            {
		                printf("- Validated\r\n");
		                printf("Saving block %d to slot %d\r\n", curBlock.blockId, curImgSlot);
		                if(curBlock.type == DATATYPE_FW_UPDATE)
		                    saveUpdateBlockData(curBlock.blockId);
		                else
		                	saveImgBlockData(curImgSlot, curBlock.blockId);
		                curBlock.blockId++;
		                curDataInfo.dataSize -= dataRequestSizeBLE;
		                if (!curDataInfo.dataSize)
		                {
			                if(curBlock.type == DATATYPE_FW_UPDATE){
			                    printf("firmware download complete, doing update.\r\n");
			                	ble_send_notify(BLE_CMD_ACK_FW_UPDATED, NULL,NULL);
			                    wdt60s();
			            		WaitMs(100);
			            		doRebootOrFirmwareUpdate = 0x13371338;
			                    //test_firmware_writing();
			                }else{
			            		ble_set_connection_speed(400);
			                    struct EepromImageHeader *eih = (struct EepromImageHeader *)blockXferBuffer;
			                    memcpy(&eih->version, &curDataInfo.dataVer, 8);
			                    eih->validMarker = EEPROM_IMG_VALID + settings.saved_EEPROM_IMG_EACH;
			                    eih->id = ++curHighSlotId;
			                    eih->size = imageSize;
			                    eih->dataType = curDataInfo.dataType;
			                    printf("Now writing datatype 0x%02X to slot %d\r\n", curDataInfo.dataType, curImgSlot);
			                    eepromWrite(getAddressForSlot(curImgSlot), (uint8_t*)eih, sizeof(struct EepromImageHeader));
			                    printf("download complete!\r\n");
			                	ble_send_notify(BLE_CMD_ACK, NULL,NULL);
			                    wdt60s();
			                    drawOnOffline(1);
			                    drawImageFromEeprom(curImgSlot);
			                }
		                }else{
			                memset(curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
			                if (curDataInfo.dataSize > BLOCK_DATA_SIZE)
			                {
			                    // more than one block remaining
			                	dataRequestSizeBLE = BLOCK_DATA_SIZE;
			                }
			                else
			                {
			                    // only one block remains
			                	dataRequestSizeBLE = curDataInfo.dataSize;
			                }
			                	partsThisBlockBLE = (sizeof(struct blockData) + dataRequestSizeBLE) / BLOCK_PART_DATA_SIZE_BLE;
			                    if ((sizeof(struct blockData) + dataRequestSizeBLE) % BLOCK_PART_DATA_SIZE_BLE)
			                    	partsThisBlockBLE++;
			                printf("Request next block! %u %08X \r\n", partsThisBlockBLE, curDataInfo.dataSize);
			            	ble_send_notify(BLE_CMD_REQ, (uint8_t *)&curBlock,sizeof(struct blockRequest));
		                }
		            }
		            else
		            {
		                memset(curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
		                printf("blk failed validation!\r\n");
		                if (curDataInfo.dataSize > BLOCK_DATA_SIZE)
		                {
		                    // more than one block remaining
		                	dataRequestSizeBLE = BLOCK_DATA_SIZE;
		                }
		                else
		                {
		                    // only one block remains
		                	dataRequestSizeBLE = curDataInfo.dataSize;
		                }
		                	partsThisBlockBLE = (sizeof(struct blockData) + dataRequestSizeBLE) / BLOCK_PART_DATA_SIZE_BLE;
		                    if ((sizeof(struct blockData) + dataRequestSizeBLE) % BLOCK_PART_DATA_SIZE_BLE)
		                    	partsThisBlockBLE++;
		                printf("Request next block! %u %08X \r\n", partsThisBlockBLE, curDataInfo.dataSize);
		            	ble_send_notify(BLE_CMD_REQ, (uint8_t *)&curBlock,sizeof(struct blockRequest));
		            }
		        }else{
		            	ble_send_notify(BLE_CMD_ACK_BLKPRT, NULL,NULL);
		            }
	}else{
    	ble_send_notify(BLE_CMD_ERR_BLKPRT, NULL,NULL);
        printf("Something is wrong with the packet!\r\n");
        for(int i = 0;i<len;i++)
        {
        	printf(" %02X",payload[i]);
        }
    	printf("\r\n");
	}*/
}

