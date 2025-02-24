/********************************** (C) COPYRIGHT *******************************
 * File Name          : broadcaster.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : 广播应用程序，初始化广播连接参数，然后处于广播态一直广播

 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "main.h"
#include "config.h"
#include "peripheral.h"
#include "gattprofile.h"
#include "tl_common.h"

#include "HAL.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// How often to perform periodic event
#define SBP_PERIODIC_EVT_PERIOD 1600

// Parameter update delay
#define SBP_PARAM_UPDATE_DELAY 2600

// What is the advertising interval when device is discoverable (units of 625us, 80=50ms)
#define DEFAULT_ADVERTISING_INTERVAL 800 * 4

// Minimum connection interval (units of 1.25ms, 6=7.5ms)
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL 400

// Maximum connection interval (units of 1.25ms, 100=125ms)
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL 400

// Slave latency to use parameter update
#define DEFAULT_DESIRED_SLAVE_LATENCY 0

// Supervision timeout value (units of 10ms, 100=1s)
#define DEFAULT_DESIRED_CONN_TIMEOUT 3200

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 Peripheral_TaskID = INVALID_TASK_ID;  // Task ID for internal task/event processing

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8 scanRspData[] =
    {
        // complete name
        11,  // length of this data
        GAP_ADTYPE_LOCAL_NAME_COMPLETE,
        'A',
        'T',
        'C',
        '_',
        'X',
        'X',
        'X',
        'X',
        'X',
        'X',
};

struct __attribute__ ((packed)) BleAdvDataStruct {
    uint8_t len_cap;           // needs to be 2
    uint8_t type_cap;          // needs to be 1
    uint8_t capabilities_cap;  // needs to be 5
    uint8_t len;               // Len of all
    uint8_t type;              // Always 0xff
    uint16_t manu_id;          // 0x1337 for us
    uint8_t version;
    uint16_t hw_type;
    uint16_t fw_version;
    uint16_t capabilities;
    uint16_t battery_mv;
    uint8_t counter;
};

struct BleAdvDataStruct ble_adv_data;

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertising)
static uint8 advertData[] =
    {
        // Flags; this sets the device to use limited discoverable
        // mode (advertises for 30 seconds at a time) instead of general
        // discoverable mode (advertises indefinitely)
        0x02,  // length of this data
        GAP_ADTYPE_FLAGS,
        GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
};

// GAP GATT Attributes
static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "ATC_XXXXXX";

// Connection item list
static peripheralConnItem_t peripheralConnList;

static uint8_t peripheralMTU = ATT_MTU_SIZE;
/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void Peripheral_ProcessTMOSMsg (tmos_event_hdr_t *pMsg);
static void peripheralStateNotificationCB (gapRole_States_t newState, gapRoleEvent_t *pEvent);
static void performPeriodicTask (void);
static void simpleProfileChangeCB (uint8 paramID, uint8 *pValue, uint16 len);
static void peripheralParamUpdateCB (uint16 connHandle, uint16 connInterval,
                                     uint16 connSlaveLatency, uint16 connTimeout);
static void peripheralInitConnItem (peripheralConnItem_t *peripheralConnList);
static void peripheralChar4Notify (uint8 *pValue, uint16 len);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t Peripheral_PeripheralCBs =
    {
        peripheralStateNotificationCB,  // Profile State Change Callbacks
        NULL,                           // When a valid RSSI is read from controller (not used by application)
        peripheralParamUpdateCB};

// Broadcast Callbacks
static gapRolesBroadcasterCBs_t Broadcaster_BroadcasterCBs =
    {
        NULL,  // Not used in peripheral role
        NULL   // Receive scan request callback
};

// GAP Bond Manager Callbacks
static gapBondCBs_t Peripheral_BondMgrCBs =
    {
        NULL,  // Passcode callback (not used by application)
        NULL   // Pairing / Bonding state Callback (not used by application)
};

// Simple GATT Profile Callbacks
static simpleProfileCBs_t Peripheral_SimpleProfileCBs =
    {
        simpleProfileChangeCB  // Characteristic value change callback
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Peripheral_Init
 *
 * @brief   Initialization function for the Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by TMOS.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void Peripheral_Init() {
    uint8_t MacAddr[6];
    GetMACAddress (MacAddr);
    
	const char *hex_ascii = {"0123456789ABCDEF"};
	attDeviceName[4] = hex_ascii[MacAddr[2] >> 4];
	attDeviceName[5] = hex_ascii[MacAddr[2] & 0x0f];
	attDeviceName[6] = hex_ascii[MacAddr[1] >> 4];
	attDeviceName[7] = hex_ascii[MacAddr[1] & 0x0f];
	attDeviceName[8] = hex_ascii[MacAddr[0] >> 4];
	attDeviceName[9] = hex_ascii[MacAddr[0] & 0x0f];

    scanRspData[6] = hex_ascii[MacAddr[2] >> 4];
    scanRspData[7] = hex_ascii[MacAddr[2] & 0x0f];
    scanRspData[8] = hex_ascii[MacAddr[1] >> 4];
    scanRspData[9] = hex_ascii[MacAddr[1] & 0x0f];
    scanRspData[10] = hex_ascii[MacAddr[0] >> 4];
    scanRspData[11] = hex_ascii[MacAddr[0] & 0x0f];

    Peripheral_TaskID = TMOS_ProcessEventRegister (Peripheral_ProcessEvent);

    // Setup the GAP Peripheral Role Profile
    {
        uint8 initial_advertising_enable = TRUE;
        uint16 desired_min_interval = 8;
        uint16 desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;

        // Set the GAP Role Parameters
        GAPRole_SetParameter (GAPROLE_ADVERT_ENABLED, sizeof (uint8), &initial_advertising_enable);
        GAPRole_SetParameter (GAPROLE_SCAN_RSP_DATA, sizeof (scanRspData), scanRspData);
        GAPRole_SetParameter (GAPROLE_ADVERT_DATA, sizeof (advertData), advertData);
        GAPRole_SetParameter (GAPROLE_MIN_CONN_INTERVAL, sizeof (uint16), &desired_min_interval);
        GAPRole_SetParameter (GAPROLE_MAX_CONN_INTERVAL, sizeof (uint16), &desired_max_interval);
    }

    // Set the GAP Characteristics
    GGS_SetParameter (GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

    // Set advertising interval
    {
        uint16 advInt = DEFAULT_ADVERTISING_INTERVAL;

        GAP_SetParamValue (TGAP_DISC_ADV_INT_MIN, advInt);
        GAP_SetParamValue (TGAP_DISC_ADV_INT_MAX, advInt);
    }

    // Setup the GAP Bond Manager
    {
        uint32 passkey = 0;  // passkey "000000"
        uint8 pairMode = GAPBOND_PAIRING_MODE_NO_PAIRING;
        uint8 mitm = TRUE;
        uint8 bonding = TRUE;
        uint8 ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
        GAPBondMgr_SetParameter (GAPBOND_PERI_DEFAULT_PASSCODE, sizeof (uint32), &passkey);
        GAPBondMgr_SetParameter (GAPBOND_PERI_PAIRING_MODE, sizeof (uint8), &pairMode);
        GAPBondMgr_SetParameter (GAPBOND_PERI_MITM_PROTECTION, sizeof (uint8), &mitm);
        GAPBondMgr_SetParameter (GAPBOND_PERI_IO_CAPABILITIES, sizeof (uint8), &ioCap);
        GAPBondMgr_SetParameter (GAPBOND_PERI_BONDING_ENABLED, sizeof (uint8), &bonding);
    }

    // Initialize GATT attributes
    GGS_AddService (GATT_ALL_SERVICES);            // GAP
    GATTServApp_AddService (GATT_ALL_SERVICES);    // GATT attributes
    SimpleProfile_AddService (GATT_ALL_SERVICES);  // Simple GATT Profile

    // Setup the SimpleProfile Characteristic Values
    {
        uint8 charValue1[SIMPLEPROFILE_CHAR1_LEN] = {1};

        SimpleProfile_SetParameter (SIMPLEPROFILE_CHAR1, SIMPLEPROFILE_CHAR1_LEN, charValue1);
    }

    // Init Connection Item
    peripheralInitConnItem (&peripheralConnList);

    // Register callback with SimpleGATTprofile
    SimpleProfile_RegisterAppCBs (&Peripheral_SimpleProfileCBs);

    // Register receive scan request callback
    GAPRole_BroadcasterSetCB (&Broadcaster_BroadcasterCBs);

    // Setup a delayed profile startup
    tmos_set_event (Peripheral_TaskID, SBP_START_DEVICE_EVT);
}

/*********************************************************************
 * @fn      peripheralInitConnItem
 *
 * @brief   Init Connection Item
 *
 * @param   peripheralConnList -
 *
 * @return  NULL
 */
static void peripheralInitConnItem (peripheralConnItem_t *peripheralConnList) {
    peripheralConnList->connHandle = GAP_CONNHANDLE_INIT;
    peripheralConnList->connInterval = 0;
    peripheralConnList->connSlaveLatency = 0;
    peripheralConnList->connTimeout = 0;
}

/*********************************************************************
 * @fn      Peripheral_ProcessEvent
 *
 * @brief   Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 Peripheral_ProcessEvent (uint8 task_id, uint16 events) {

    //  VOID task_id; // TMOS required parameter that isn't used in this function

    if (events & SYS_EVENT_MSG) {
        uint8 *pMsg;

        if ((pMsg = tmos_msg_receive (Peripheral_TaskID)) != NULL) {
            Peripheral_ProcessTMOSMsg ((tmos_event_hdr_t *)pMsg);
            // Release the TMOS message
            tmos_msg_deallocate (pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if (events & SBP_START_DEVICE_EVT) {
        // Start the Device
        GAPRole_PeripheralStartDevice (Peripheral_TaskID, &Peripheral_BondMgrCBs, &Peripheral_PeripheralCBs);
        return (events ^ SBP_START_DEVICE_EVT);
    }

    if (events & SBP_PERIODIC_EVT) {
        // Restart timer
        if (SBP_PERIODIC_EVT_PERIOD) {
            tmos_start_task (Peripheral_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD);
        }
        // Perform periodic application task
        performPeriodicTask();
        return (events ^ SBP_PERIODIC_EVT);
    }

    /*if (events & SBP_PARAM_UPDATE_EVT) {
        // Send connect param update request
        GAPRole_PeripheralConnParamUpdateReq (peripheralConnList.connHandle, DEFAULT_DESIRED_MIN_CONN_INTERVAL, DEFAULT_DESIRED_MAX_CONN_INTERVAL, DEFAULT_DESIRED_SLAVE_LATENCY, DEFAULT_DESIRED_CONN_TIMEOUT, Peripheral_TaskID);

        return (events ^ SBP_PARAM_UPDATE_EVT);
    }*/

    // Discard unknown events
    return 0;
}

/*********************************************************************
 * @fn      Peripheral_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void Peripheral_ProcessTMOSMsg (tmos_event_hdr_t *pMsg) {
    switch (pMsg->event) {
    case GATT_MSG_EVENT: {
        gattMsgEvent_t *pMsgEvent;

        pMsgEvent = (gattMsgEvent_t *)pMsg;
        if (pMsgEvent->method == ATT_MTU_UPDATED_EVENT) {
            peripheralMTU = pMsgEvent->msg.exchangeMTUReq.clientRxMTU;
            PRINT ("mtu exchange: %d\n", pMsgEvent->msg.exchangeMTUReq.clientRxMTU);
        }
        break;
    }
    default:
        break;
    }
}

/*********************************************************************
 * @fn      Peripheral_LinkEstablished
 *
 * @brief   Process link established.
 *
 * @param   pEvent - event to process
 *
 * @return  none
 */
static void Peripheral_LinkEstablished (gapRoleEvent_t *pEvent) {
    gapEstLinkReqEvent_t *event = (gapEstLinkReqEvent_t *)pEvent;

    // See if already connected
    if (peripheralConnList.connHandle != GAP_CONNHANDLE_INIT) {
        GAPRole_TerminateLink (pEvent->linkCmpl.connectionHandle);
        PRINT ("Connection max...\n");
    } else {
        peripheralConnList.connHandle = event->connectionHandle;
        peripheralConnList.connInterval = event->connInterval;
        peripheralConnList.connSlaveLatency = event->connLatency;
        peripheralConnList.connTimeout = event->connTimeout;
        peripheralMTU = ATT_MTU_SIZE;
        // Set timer for periodic event
        // tmos_start_task (Peripheral_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD);

        // Set timer for param update event
        //tmos_start_task (Peripheral_TaskID, SBP_PARAM_UPDATE_EVT, SBP_PARAM_UPDATE_DELAY);

        PRINT ("Conn %x - Int %x \n", event->connectionHandle, event->connInterval);
    }
}

/*********************************************************************
 * @fn      Peripheral_LinkTerminated
 *
 * @brief   Process link terminated.
 *
 * @param   pEvent - event to process
 *
 * @return  none
 */
static void Peripheral_LinkTerminated (gapRoleEvent_t *pEvent) {
    gapTerminateLinkEvent_t *event = (gapTerminateLinkEvent_t *)pEvent;

    if (event->connectionHandle == peripheralConnList.connHandle) {
        peripheralConnList.connHandle = GAP_CONNHANDLE_INIT;
        peripheralConnList.connInterval = 0;
        peripheralConnList.connSlaveLatency = 0;
        peripheralConnList.connTimeout = 0;
        tmos_stop_task (Peripheral_TaskID, SBP_PERIODIC_EVT);

        // Restart advertising
        {
            uint8 advertising_enable = TRUE;
            GAPRole_SetParameter (GAPROLE_ADVERT_ENABLED, sizeof (uint8), &advertising_enable);
        }
    } else {
        PRINT ("ERR..\n");
    }
}

/*********************************************************************
 * @fn      peripheralParamUpdateCB
 *
 * @brief   Parameter update complete callback
 *
 * @param   connHandle - connect handle
 *          connInterval - connect interval
 *          connSlaveLatency - connect slave latency
 *          connTimeout - connect timeout
 *
 * @return  none
 */
static void peripheralParamUpdateCB (uint16 connHandle, uint16 connInterval,
                                     uint16 connSlaveLatency, uint16 connTimeout) {
    if (connHandle == peripheralConnList.connHandle) {
        peripheralConnList.connInterval = connInterval;
        peripheralConnList.connSlaveLatency = connSlaveLatency;
        peripheralConnList.connTimeout = connTimeout;

        PRINT ("Update %x - Int %x \n", connHandle, connInterval);
    } else {
        PRINT ("ERR..\n");
    }
}

/*********************************************************************
 * @fn      peripheralStateNotificationCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void peripheralStateNotificationCB (gapRole_States_t newState, gapRoleEvent_t *pEvent) {
    switch (newState) {
    case GAPROLE_STARTED:
        PRINT ("Initialized..\n");
        break;

    case GAPROLE_ADVERTISING:
        if (pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT) {
            Peripheral_LinkTerminated (pEvent);
            PRINT ("Disconnected.. Reason:%x\n", pEvent->linkTerminate.reason);
        }
        PRINT ("Advertising..\n");
        set_adv_data (3000);
        break;

    case GAPROLE_CONNECTED:
        if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT) {
            Peripheral_LinkEstablished (pEvent);
        }
        PRINT ("Connected..\n");
        break;

    case GAPROLE_CONNECTED_ADV:
        PRINT ("Connected Advertising..\n");
        break;

    case GAPROLE_WAITING:
        if (pEvent->gap.opcode == GAP_END_DISCOVERABLE_DONE_EVENT) {
            PRINT ("Waiting for advertising..\n");
            uint8 advertising_enable = TRUE;
            GAPRole_SetParameter (GAPROLE_ADVERT_ENABLED, sizeof (uint8), &advertising_enable);
        } else if (pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT) {
            Peripheral_LinkTerminated (pEvent);
            PRINT ("Disconnected.. Reason:%x\n", pEvent->linkTerminate.reason);
        } else if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT) {
            if (pEvent->gap.hdr.status != SUCCESS) {
                PRINT ("Waiting for advertising.. Err\n");
                uint8 advertising_enable = TRUE;
                GAPRole_SetParameter (GAPROLE_ADVERT_ENABLED, sizeof (uint8), &advertising_enable);
            } else {
                PRINT ("Error..\n");
            }
        } else {
            PRINT ("Error..%x\n", pEvent->gap.opcode);
        }
        break;

    case GAPROLE_ERROR:
        PRINT ("Error..\n");
        break;

    default:
        break;
    }
}

/*********************************************************************
 * @fn      performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets
 *          called every five seconds as a result of the SBP_PERIODIC_EVT
 *          TMOS event. In this example, the value of the third
 *          characteristic in the SimpleGATTProfile service is retrieved
 *          from the profile, and then copied into the value of the
 *          the fourth characteristic.
 *
 * @param   none
 *
 * @return  none
 */
uint8_t aaa = 0;

static void performPeriodicTask (void) {
    // uint8 notiData[SIMPLEPROFILE_CHAR1_LEN] = {aaa++};
    // peripheralChar4Notify (notiData, SIMPLEPROFILE_CHAR1_LEN);
    PRINT ("Periodic task\n");
}

static void peripheralChar4Notify (uint8 *pValue, uint16 len) {
    attHandleValueNoti_t noti;
    if (len > (peripheralMTU - 3)) {
        PRINT ("Too large noti\n");
        return;
    }
    noti.len = len;
    noti.pValue = GATT_bm_alloc (peripheralConnList.connHandle, ATT_HANDLE_VALUE_NOTI, noti.len, NULL, 0);
    tmos_memcpy (noti.pValue, pValue, noti.len);
    if (simpleProfile_Notify (peripheralConnList.connHandle, &noti) != SUCCESS) {
        GATT_bm_free ((gattMsg_t *)&noti, ATT_HANDLE_VALUE_NOTI);
    }
}

static uint8_t bleNotifyBuff[260];

void ble_send_notify (uint16_t cmd, uint8_t *data, uint8_t len) {
    printf ("Sending %u of notify\r\n", len + 2);
    if (len > sizeof (bleNotifyBuff) - 2)
        return;
    bleNotifyBuff[0] = cmd >> 8;
    bleNotifyBuff[1] = cmd & 0xff;
    memcpy (&bleNotifyBuff[2], data, len);
    peripheralChar4Notify (bleNotifyBuff, len + 2);
}

void ble_set_connection_speed (uint16_t speed) {
    printf ("Updating speed: %u\r\n", speed);
    /*GAPRole_PeripheralConnParamUpdateReq (peripheralConnList.connHandle,
                                          speed,
                                          speed + 2,
                                          DEFAULT_DESIRED_SLAVE_LATENCY,
                                          DEFAULT_DESIRED_CONN_TIMEOUT,
                                          Peripheral_TaskID);*/
}

void set_adv_data (uint16_t battery_mv) {
    ble_adv_data.len_cap = 2;
    ble_adv_data.type_cap = 1;
    ble_adv_data.capabilities_cap = 5;
    ble_adv_data.len = sizeof (struct BleAdvDataStruct) - 4;
    ble_adv_data.type = 0xFF;
    ble_adv_data.manu_id = 0x1337;
    ble_adv_data.version = 1;
    ble_adv_data.hw_type = settings.oepl_hw_type;
    ble_adv_data.fw_version = FIRMWARE_VERSION;
    ble_adv_data.capabilities = 0;  // capabilities;
    ble_adv_data.battery_mv = battery_mv;
    ble_adv_data.counter++;
    GAP_UpdateAdvertisingData (Peripheral_TaskID, TRUE, sizeof (struct BleAdvDataStruct), (uint8_t *)&ble_adv_data);
}

/*********************************************************************
 * @fn      simpleProfileChangeCB
 *
 * @brief   Callback from SimpleBLEProfile indicating a value change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *          pValue - pointer to data that was changed
 *          len - length of data
 *
 * @return  none
 */
uint8 newValue[SIMPLEPROFILE_CHAR1_LEN];

static void simpleProfileChangeCB (uint8 paramID, uint8 *pValue, uint16 len) {

    switch (paramID) {
    case SIMPLEPROFILE_CHAR1: {
        tmos_memcpy (newValue, pValue, len);
        u16 cmd_type = newValue[0];
        cmd_type <<= 8;
        cmd_type |= newValue[1];
        extern int zb_ble_hci_cmd_handler (u16 cmdId, u8 len, u8 * payload);
        zb_ble_hci_cmd_handler (cmd_type, len, &(newValue[2]));
        break;
    }
    default:
        // should not reach here!
        break;
    }
}

/*********************************************************************
*********************************************************************/
