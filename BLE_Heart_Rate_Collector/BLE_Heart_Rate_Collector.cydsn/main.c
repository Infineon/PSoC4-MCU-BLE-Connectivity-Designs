/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This project demonstrates the operation of the Heart Rate Profile
*  in Collector (Central) role.
*
* Related Document:
*  HEART RATE PROFILE SPECIFICATION v1.0
*  HEART RATE SERVICE SPECIFICATION v1.0
*
*******************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "main.h"


uint8 flag = 0;
CYBLE_API_RESULT_T apiResult;
uint8 advDevices = 0u;
uint8 deviceN = 0u;
CYBLE_UUID16 serviceUuid = 0x0000u;
CYBLE_GAP_BD_ADDR_T peerAddr[CYBLE_MAX_ADV_DEVICES];


/*******************************************************************************
* Function Name: AppCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
*******************************************************************************/

void AppCallBack(uint32 event, void* eventParam)
{
    uint16 i;
    switch(event)
    {
        case CYBLE_EVT_STACK_ON:
            DBG_PRINTF("CYBLE_EVT_STACK_ON \r\n");
            StartScan(CYBLE_UUID_HEART_RATE_SERVICE);
            break;

        case CYBLE_EVT_TIMEOUT: /* 0x01 -> GAP limited discoverable mode timeout. */
                                /* 0x02 -> GAP pairing process timeout. */
                                /* 0x03 -> GATT response timeout. */
            DBG_PRINTF("CYBLE_EVT_TIMEOUT: %d \r\n", *(uint8 *)eventParam);
            break;

        case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred. */
            DBG_PRINTF("CYBLE_EVT_HARDWARE_ERROR \r\n");
            break;

        case CYBLE_EVT_HCI_STATUS:
            DBG_PRINTF("CYBLE_EVT_HCI_STATUS %d \r\n", *(uint8 *)eventParam);
            break;

        /**********************************************************
        *                       GAP Events
        ***********************************************************/

        case CYBLE_EVT_GAP_AUTH_REQ:
            DBG_PRINTF("CYBLE_EVT_GAP_AUTH_REQ");
            break;

        case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            DBG_PRINTF("CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST");
            break;

        case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            DBG_PRINTF("CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST: %6.6ld \r\n", *(uint32 *)eventParam);
            break;

        case CYBLE_EVT_GAP_AUTH_FAILED:
            DBG_PRINTF("CYBLE_EVT_GAP_AUTH_FAILED, reason: ");
            switch(*(CYBLE_GAP_AUTH_FAILED_REASON_T *)eventParam)
            {
                case CYBLE_GAP_AUTH_ERROR_CONFIRM_VALUE_NOT_MATCH:
                    DBG_PRINTF("CONFIRM_VALUE_NOT_MATCH\r\n");
                    break;
                    
                case CYBLE_GAP_AUTH_ERROR_INSUFFICIENT_ENCRYPTION_KEY_SIZE:
                    DBG_PRINTF("INSUFFICIENT_ENCRYPTION_KEY_SIZE\r\n");
                    break;
                
                case CYBLE_GAP_AUTH_ERROR_UNSPECIFIED_REASON:
                    DBG_PRINTF("UNSPECIFIED_REASON\r\n");
                    break;
                    
                case CYBLE_GAP_AUTH_ERROR_AUTHENTICATION_TIMEOUT:
                    DBG_PRINTF("AUTHENTICATION_TIMEOUT\r\n");
                    break;
                    
                default:
                    DBG_PRINTF("0x%x  \r\n", *(CYBLE_GAP_AUTH_FAILED_REASON_T *)eventParam);
                    break;
            }
            break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_CONNECTED: %x \r\n", cyBle_connHandle.bdHandle);
            CyBle_GapAddDeviceToWhiteList(&peerAddr[deviceN]);
            /* Send authorization request. */
            apiResult = CyBle_GapAuthReq(cyBle_connHandle.bdHandle, &cyBle_authInfo);
            
            if(CYBLE_ERROR_OK != apiResult)
        	{
        		DBG_PRINTF("CyBle_GapAuthReq API Error: ");
                PrintApiResult();
        	}
        	else
        	{
        		DBG_PRINTF("Authentification request is sent \r\n");
        	}

            Disconnect_LED_Write(LED_OFF);
            Scanning_LED_Write(LED_OFF);
            break;

        case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE \r\n");
            break;

        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_DISCONNECTED, reason: %x \r\n", *(uint8*)eventParam);
            Disconnect_LED_Write(LED_ON);
            StartScan(CYBLE_UUID_HEART_RATE_SERVICE);
            break;

        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GAP_AUTH_COMPLETE: security:%x, bonding:%x, ekeySize:%x, authErr %x \r\n",
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->security,
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->bonding, 
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->ekeySize, 
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->authErr);
            StartDiscovery();
            break;

        case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
            DBG_PRINTF("CYBLE_EVT_GAP_ENCRYPT_CHANGE: %d \r\n", *(uint8 *)eventParam);
            break;
            
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            ScanProgressEventHandler((CYBLE_GAPC_ADV_REPORT_T *)eventParam);
            break;

        case CYBLE_EVT_GAPC_SCAN_START_STOP:
            DBG_PRINTF("CYBLE_EVT_GAPC_SCAN_START_STOP\r\n");
            PrintState();
            
            Disconnect_LED_Write(LED_OFF);
            if(0u != (flag & CONNECT))
            {
                DBG_PRINTF("Connect to the Device: %x \r\n", deviceN);
                /* Connect to selected device */
                apiResult = CyBle_GapcConnectDevice(&peerAddr[deviceN]);
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("ConnectDevice API Error: ");
                    PrintApiResult();
                }

                flag &= ~CONNECT;
            }
            else if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
            {   
                /* Scanning period complete, go to low power  
                 * mode (Hibernate mode) and wait for an external
                 * user event to wake up the device again */
                DBG_PRINTF("Hibernate \r\n");
                Disconnect_LED_Write(LED_ON);
                Scanning_LED_Write(LED_OFF);
                Notification_LED_Write(LED_OFF);
            #if (DEBUG_UART_ENABLED == ENABLED)
                while((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) != 0);
            #endif /* (DEBUG_UART_ENABLED == ENABLED) */
                SW2_ClearInterrupt();
                Wakeup_Interrupt_ClearPending();
                Wakeup_Interrupt_Start();
                CySysPmHibernate();
            }
            break;

        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CYBLE_EVT_GATTC_ERROR_RSP:
            DBG_PRINTF("CYBLE_EVT_GATTC_ERROR_RSP: opcode: ");
            switch(((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->opCode)
            {
                case CYBLE_GATT_FIND_INFO_REQ:
                    DBG_PRINTF("FIND_INFO_REQ");
                    break;

                case CYBLE_GATT_READ_BY_TYPE_REQ:
                    DBG_PRINTF("READ_BY_TYPE_REQ");
                    break;

                case CYBLE_GATT_READ_BY_GROUP_REQ:
                    DBG_PRINTF("READ_BY_GROUP_REQ");
                    break;

                default:
                    DBG_PRINTF("%x", ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->opCode);
                    break;
            }
            DBG_PRINTF(",  handle: %x,  ", ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->attrHandle);
            DBG_PRINTF("errorcode: ");
            switch(((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->errorCode)
            {
                case CYBLE_GATT_ERR_ATTRIBUTE_NOT_FOUND:
                    DBG_PRINTF("ATTRIBUTE_NOT_FOUND");
                    break;
                    
                case CYBLE_GATT_ERR_READ_NOT_PERMITTED:
                    DBG_PRINTF("READ_NOT_PERMITTED");
                    break;

                default:
                    DBG_PRINTF("%x", ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->errorCode);
                    break;
            }
            DBG_PRINTF("\r\n");
            break;

        case CYBLE_EVT_GATT_CONNECT_IND:
            DBG_PRINTF("CYBLE_EVT_GATT_CONNECT_IND: attId %x, bdHandle %x \r\n", 
                ((CYBLE_CONN_HANDLE_T *)eventParam)->attId, ((CYBLE_CONN_HANDLE_T *)eventParam)->bdHandle);
            break;

        case CYBLE_EVT_GATT_DISCONNECT_IND:
            DBG_PRINTF("CYBLE_EVT_GATT_DISCONNECT_IND \r\n");
            break;
            
        case CYBLE_EVT_GATTC_READ_RSP:
            DBG_PRINTF("CYBLE_EVT_GATTC_READ_RSP: ");
            for(i = 0; i < ((CYBLE_GATTC_READ_RSP_PARAM_T *)eventParam)->value.len; i++)
            {
                DBG_PRINTF("%2.2x ", ((CYBLE_GATTC_READ_RSP_PARAM_T *)eventParam)->value.val[i]);
            }
            DBG_PRINTF("\r\n");
            break;

        case CYBLE_EVT_GATTC_WRITE_RSP:
            DBG_PRINTF("CYBLE_EVT_GATTC_WRITE_RSP: ");
            DBG_PRINTF("bdHandle: %x \r\n", ((CYBLE_CONN_HANDLE_T *)eventParam)->bdHandle);
            break;

        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
            DBG_PRINTF("CYBLE_EVT_GATTC_XCHNG_MTU_RSP \r\n");
            break;
            
        case CYBLE_EVT_GATTC_SRVC_DUPLICATION:
            DBG_PRINTF("CYBLE_EVT_GATTC_SRVC_DUPLICATION, UUID: %x \r\n", *(uint16 *)eventParam);
            break;

        case CYBLE_EVT_GATTC_CHAR_DUPLICATION:
            DBG_PRINTF("CYBLE_EVT_GATTC_CHAR_DUPLICATION, UUID: %x \r\n", *(uint16 *)eventParam);
            break;

        case CYBLE_EVT_GATTC_DESCR_DUPLICATION:
            DBG_PRINTF("CYBLE_EVT_GATTC_DESCR_DUPLICATION, UUID: %x \r\n", *(uint16 *)eventParam);
            break;

        case CYBLE_EVT_GATTC_SRVC_DISCOVERY_FAILED:
            DBG_PRINTF("CYBLE_EVT_GATTC_DISCOVERY_FAILED \r\n");
            break;

        case CYBLE_EVT_GATTC_SRVC_DISCOVERY_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GATTC_SRVC_DISCOVERY_COMPLETE \r\n");
            break;

        case CYBLE_EVT_GATTC_INCL_DISCOVERY_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GATTC_INCL_DISCOVERY_COMPLETE \r\n");
            break;

        case CYBLE_EVT_GATTC_CHAR_DISCOVERY_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GATTC_CHAR_DISCOVERY_COMPLETE \r\n");
            break;
            
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GATTC_DISCOVERY_COMPLETE\r\n");
            /* Send request to read the body sensor location char. */
            apiResult = HrscReadBodySensorLocation();

            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("HrscReadBodySensorLocation API Error: ");
                PrintApiResult();
            }
            else
            {
                DBG_PRINTF("Body Sensor Location Read Request is sent \r\n");
            }
            break;
            
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            DBG_PRINTF("CYBLE_EVT_GATTC_HANDLE_VALUE_NTF \r\n");
            break;


        /**********************************************************
        *                       L2CAP Events
        ***********************************************************/

        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ:
            DBG_PRINTF("CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ: %x, %x, %x, %x \r\n", 
                ((CYBLE_GAP_CONN_UPDATE_PARAM_T *)eventParam)->connIntvMax,
                ((CYBLE_GAP_CONN_UPDATE_PARAM_T *)eventParam)->connIntvMin,
                ((CYBLE_GAP_CONN_UPDATE_PARAM_T *)eventParam)->connLatency,
                ((CYBLE_GAP_CONN_UPDATE_PARAM_T *)eventParam)->supervisionTO
            );
            /* Accepted = 0x0000, Rejected  = 0x0001 */
            CyBle_L2capLeConnectionParamUpdateResponse(cyBle_connHandle.bdHandle, 0u);
            break;

        /**********************************************************
        *                       Other Events
        ***********************************************************/
        case CYBLE_EVT_PENDING_FLASH_WRITE:
            /* Inform application that flash write is pending. Stack internal data 
            * structures are modified and require to be stored in Flash using 
            * CyBle_StoreBondingData() */
            DBG_PRINTF("CYBLE_EVT_PENDING_FLASH_WRITE\r\n");
            break;

        default:
            DBG_PRINTF("Other event: %lx \r\n", event);
            break;
    }
}


/*******************************************************************************
* Function Name: StartScan
********************************************************************************
*
* Summary:
*  Starts the Scanning process.
*
*******************************************************************************/
void StartScan(CYBLE_UUID16 uuid)
{
    serviceUuid = uuid;
    apiResult = CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
	if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("StartScan API Error: ");
        PrintApiResult();
    }
    else
    {
        DBG_PRINTF("Start Scan \r\n");
    }
}


/*******************************************************************************
* Function Name: StartDiscovery
********************************************************************************
*
* Summary:
*  Starts the Server Discovery process.
*
*******************************************************************************/
void StartDiscovery(void)
{
    apiResult = CyBle_GattcStartDiscovery(cyBle_connHandle);

    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("CyBle_GattcStartDiscovery API Error: ");
        PrintApiResult();
    }
    else
    {
        DBG_PRINTF("Start Discovery \r\n");
    }
}


/*******************************************************************************
* Function Name: ScanProgressEventHandler
********************************************************************************
*
* Summary:
*  Handles the CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT event data.
*
*******************************************************************************/
void ScanProgressEventHandler(CYBLE_GAPC_ADV_REPORT_T* eventParam)
{
    uint8 newDevice = 0u, device = 0u;
    uint8 i;
    uint8 adStructPtr = 0u;
    uint8 adTypePtr = 0u;
    uint8 nextPtr = 0u;

    DBG_PRINTF("SCAN_PROGRESS_RESULT: peerAddrType - %d, ", eventParam->peerAddrType);
    DBG_PRINTF("peerBdAddr - ");
    for(newDevice = 1u, i = 0u; i < advDevices; i++)
    {
        if((memcmp(peerAddr[i].bdAddr, eventParam->peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE) == 0)) /* same address */
        {
            device = i;
            DBG_PRINTF("%x: ", device);
            newDevice = 0u;
            break;
        }
    }
    if(newDevice != 0u)
    {
        if(advDevices < CYBLE_MAX_ADV_DEVICES)
        {
            memcpy(peerAddr[advDevices].bdAddr, eventParam->peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE);
            peerAddr[advDevices].type = eventParam->peerAddrType;
            device = advDevices;
            advDevices++;
            DBG_PRINTF("%x: ", device);

        }
    }
    for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
    {
        DBG_PRINTF("%2.2x", eventParam->peerBdAddr[i-1]);
    }
    DBG_PRINTF(", rssi - %d dBm, data - ", eventParam->rssi);

    /* Print and parse advertisement data and connect to device which has HRM */
    adStructPtr = 0u;
    for(i = 0; i < eventParam->dataLen; i++)
    {
        DBG_PRINTF("%2.2x ", eventParam->data[i]);

        if(i == adStructPtr)
        {
            adTypePtr = i + 1;
            adStructPtr += eventParam->data[i] + 1;
            nextPtr = 1;
        }
        else if(i == (adTypePtr + nextPtr))
        {
            switch(eventParam->data[adTypePtr])
            {
                case CYBLE_GAP_ADV_FLAGS:
                    break;

                case CYBLE_GAP_ADV_INCOMPL_16UUID:
                case CYBLE_GAP_ADV_COMPL_16UUID:
                    if(serviceUuid == CyBle_Get16ByPtr(&(eventParam->data[i])))
                    {
                        newDevice = 2; /* temporary use newDevice as a flag */
                    }
                    else
                    {
                        nextPtr += 2;
                    }
                    break;

                default:
                    break;
            }
        }
    }

    if(2 == newDevice)
    {
        deviceN = device;
        DBG_PRINTF("       This device contains ");
        switch(serviceUuid)
        {
            case CYBLE_UUID_HEART_RATE_SERVICE:
                DBG_PRINTF("Heart Rate Service \r\n");
                DBG_PRINTF("Stop Scanning, waiting for Scanning event \r\n");
                flag |= CONNECT;
                CyBle_GapcStopScan();
                break;
                
                /* other services */
                
            default:
                DBG_PRINTF("%x \r\n", serviceUuid);
                break;
        }
    }
    else
    {
        DBG_PRINTF("\r\n");
    }
}


/*******************************************************************************
* Function Name: Timer_Interrupt
********************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the WDT timer.
*
*******************************************************************************/
CY_ISR(Timer_Interrupt)
{
    static uint8 led = LED_OFF;
    static uint32 ledTimer = LED_TIMEOUT;
    
    /* Blink green LED to indicate that device is scanning */
    if(CyBle_GetState() == CYBLE_STATE_SCANNING)
    {
        if(--ledTimer == 0u) 
        {
            ledTimer = LED_TIMEOUT;
            led ^= LED_OFF;
            Scanning_LED_Write(led);
        }
    }
    /* Blink blue LED to indicate that device has received a notification */
    else if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
    {
        if(LED_ON == Notification_LED_Read())
        {
            Notification_LED_Write(LED_OFF);
        }
    }
    else
    {
        /* nothing else */
    }
}


/*******************************************************************************
* Function Name: LowPowerImplementation()
********************************************************************************
* Summary:
* Implements low power in the project.
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* The function tries to enter deep sleep as much as possible - whenever the 
* BLE is idle and the UART transmission/reception is not happening. At all other
* times, the function tries to enter CPU sleep.
*
*******************************************************************************/
static void LowPowerImplementation(void)
{
    CYBLE_LP_MODE_T bleMode;
    uint8 interruptStatus;
    
    /* For advertising and connected states, implement deep sleep 
     * functionality to achieve low power in the system. For more details
     * on the low power implementation, refer to the Low Power Application 
     * Note.
     */
    if((CyBle_GetState() == CYBLE_STATE_SCANNING) || 
       (CyBle_GetState() == CYBLE_STATE_CONNECTED))
    {
        /* Request BLE subsystem to enter into Deep-Sleep mode between connection and advertising intervals */
        bleMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
        /* Disable global interrupts */
        interruptStatus = CyEnterCriticalSection();
        /* When BLE subsystem has been put into Deep-Sleep mode */
        if(bleMode == CYBLE_BLESS_DEEPSLEEP)
        {
            /* And it is still there or ECO is on */
            if((CyBle_GetBleSsState() == CYBLE_BLESS_STATE_ECO_ON) || 
               (CyBle_GetBleSsState() == CYBLE_BLESS_STATE_DEEPSLEEP))
            {
            #if (DEBUG_UART_ENABLED == ENABLED)
                /* Put the CPU into the Deep-Sleep mode when all debug information has been sent */
                if((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u)
                {
                    CySysPmDeepSleep();
                }
                else /* Put the CPU into Sleep mode and let SCB to continue sending debug data */
                {
                    CySysPmSleep();
                }
            #else
                CySysPmDeepSleep();
            #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            }
        }
        else /* When BLE subsystem has been put into Sleep mode or is active */
        {
            /* And hardware doesn't finish Tx/Rx opeation - put the CPU into Sleep mode */
            if(CyBle_GetBleSsState() != CYBLE_BLESS_STATE_EVENT_CLOSE)
            {
                CySysPmSleep();
            }
        }
        /* Enable global interrupt */
        CyExitCriticalSection(interruptStatus);
    }
}


/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
*  Main function for the project.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Theory:
*  The function starts BLE and UART components.
*  This function process all BLE events and also implements the low power 
*  functionality.
*
*******************************************************************************/
int main()
{
    CyGlobalIntEnable;
    
#if (DEBUG_UART_ENABLED == ENABLED)
	UART_DEB_Start();
#endif /* (DEBUG_UART_ENABLED == ENABLED) */
    DBG_PRINTF("\r\nBLE Heart Rate Collector Example Project\r\n");
    
    Disconnect_LED_Write(LED_OFF);
    Scanning_LED_Write(LED_OFF);
    Notification_LED_Write(LED_OFF);

    apiResult = CyBle_Start(AppCallBack);
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("CyBle_Start API Error: ");
        PrintApiResult();
    }
    
    PrintStackVersion();

    BasInit();
    HrsInit();
    
    /* Register Timer_Interrupt() by the WDT COUNTER2 to generate interrupt every second */
    CySysWdtSetInterruptCallback(CY_SYS_WDT_COUNTER2, Timer_Interrupt);
    /* Enable the COUNTER2 ISR handler. */
    CySysWdtEnableCounterIsr(CY_SYS_WDT_COUNTER2);

    while(1)
    {
        /* CyBle_ProcessEvents() allows BLE stack to process pending events */
        CyBle_ProcessEvents();

        /* To achieve low power in the device */
        LowPowerImplementation();
        
        /* Store bonding data to flash only when all debug information has been sent */
        #if (DEBUG_UART_ENABLED == ENABLED)
            if((cyBle_pendingFlashWrite != 0u) &&
               ((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u))
        #else
            if(cyBle_pendingFlashWrite != 0u)
        #endif /* (DEBUG_UART_ENABLED == ENABLED) */
        {
            apiResult = CyBle_StoreBondingData(0u);
            DBG_PRINTF("Store bonding data, status: %x \r\n", apiResult);
        }
    }
}

/* [] END OF FILE */
