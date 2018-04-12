/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This is source code for the BLE Time Sync example project.
*
* Note:
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <stdio.h>
#include "cts.h"
#include "rtus.h"
#include "ndcs.h"
#include "common.h"


/***************************************
*        Global Variables
***************************************/
CYBLE_CONN_HANDLE_T  connHandle;
uint8                advLedState = LED_OFF;
uint8                buttonState = 0u;
uint8                rtcIntOccured = FALSE;
uint8                buttonIsInUse = FALSE;


/*******************************************************************************
* Function Name: AppCallBack
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the CYBLE Component.
*
* Parameters:
*  uint8 event:       Event from the CYBLE component.
*  void* eventParams: A structure instance for corresponding event type. The
*                     list of event structure is described in the component
*                     datasheet.
*
* Return:
*  None
*
*******************************************************************************/
void AppCallBack(uint32 event, void *eventParam)
{
    uint8 i;
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GAP_AUTH_INFO_T *authInfo;
    CYBLE_GAP_BD_ADDR_T localAddr;
    CYBLE_GAP_BD_ADDR_T actPeedDevAddr;
    CYBLE_GAP_BONDED_DEV_ADDR_LIST_T bonDevList;
    
    switch(event)
    {
    /**********************************************************
    *                       General Events
    ***********************************************************/
    case CYBLE_EVT_STACK_ON: /* This event received when component is started */
        DBG_PRINTF("Bluetooth On. Start advertisement with addr:");

        CyBle_GetDeviceAddress(&localAddr);
        for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
        {
            DBG_PRINTF("%2.2x", localAddr.bdAddr[i-1]);
        }
        DBG_PRINTF(".\r\n");

        /* Starts advertisement */
        if(CYBLE_ERROR_OK == CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST))
        {
            DBG_PRINTF("Device is entered Limited Discovery mode.\r\n"); 
        }
        break;

    case CYBLE_EVT_TIMEOUT:
        DBG_PRINTF("\r\n");
        /* Possible timeout event parameter values:
        * CYBLE_GAP_ADV_MODE_TO -> GAP limited discoverable mode timeout;
        * CYBLE_GAP_AUTH_TO -> GAP pairing process timeout.
        */
        if(CYBLE_GAP_ADV_MODE_TO == *(uint8 *) eventParam)
        {
            DBG_PRINTF("Advertisement timeout occurred. Advertisement will be disabled.\r\n");
        }
        else
        {
            DBG_PRINTF("Timeout occurred.\r\n");
        }
        break;

    case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred. */
        DBG_PRINTF("\r\n");
        DBG_PRINTF("Hardware Error\r\n");
        break;

    case CYBLE_EVT_HCI_STATUS:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("HCI Error. Error code is %x.\r\n", *(uint8 *) eventParam);
        break;

    /**********************************************************
    *                       GAP Events
    ***********************************************************/
    case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
        break;

    case CYBLE_EVT_GAPC_SCAN_START_STOP:
        break;

    case CYBLE_EVT_GAP_AUTH_REQ:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_AUTH_REQ: security=0x%x, bonding=0x%x, ekeySize=0x%x, err=0x%x \r\n",
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).security,
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).bonding,
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).ekeySize,
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).authErr);
        break;
        
    case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST press 'p' to enter passkey.\r\n");
        break;

    case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_PASSKEY_DISPLAY_REQUEST %6.6ld \r\n", *(uint32 *)eventParam);
        DBG_PRINTF("Please enter the passkey on your Server device.\r\n");
        
        /* Get the BD address of currently connected device and get the list 
        * of bounded devices.
        */
        (void) CyBle_GapGetPeerBdAddr(connHandle.bdHandle, &actPeedDevAddr);        
        (void) CyBle_GapGetBondedDevicesList(&bonDevList); 

        for(i = 0u; i < bonDevList.count; i++)
        {
            /* Check if any address from the bonded devices list equals to
            * currently connected device.
            */
            if(0 == memcmp((const void *) &actPeedDevAddr, (const void *) &bonDevList.bdAddrList[i],
               sizeof(CYBLE_GAP_BD_ADDR_T)))
            {
                /* If the matching device address was found then remove the record
                * associated with current address from the bonded list as,
                * probably, the bonding infor was cleared on the peer device and
                * it is required to generate new keys. And the bounding record
                * contains stored keys.
                */
                CyBle_GapRemoveDeviceFromWhiteList(&bonDevList.bdAddrList[i]);
            }
        }
        break;

    case CYBLE_EVT_GAP_AUTH_COMPLETE:
        authInfo = (CYBLE_GAP_AUTH_INFO_T *)eventParam;
        (void)authInfo;
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_AUTH_COMPLETE: security: 0x%x, bonding: 0x%x, ekeySize: 0x%x, authErr 0x%x \r\n",
                                authInfo->security, authInfo->bonding, authInfo->ekeySize, authInfo->authErr);
        
        apiResult = CyBle_GattcStartDiscovery(connHandle);

        DBG_PRINTF("Start Discovery \r\n");
        if(apiResult != CYBLE_ERROR_OK)
        {
            DBG_PRINTF("StartDiscovery API Error: %x \r\n", apiResult);
        }
        break;

    case CYBLE_EVT_GAP_AUTH_FAILED:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_AUTH_FAILED: %x \r\n", *(uint8 *) eventParam);
        break;

    case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
        if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
        {
            DBG_PRINTF("Advertisement is disabled \r\n");
            
            /* Fast and slow advertising period complete, go to low power  
             * mode (Hibernate mode) and wait for an external
             * user event to wake up the device again */
            DBG_PRINTF("Entering Hibernate mode\r\n");
            Disc_Red_LED_Write(LED_ON);
            Adv_Green_LED_Write(LED_OFF);
            ConAct_Blue_LED_Write(LED_OFF);
        #if (DEBUG_UART_ENABLED == ENABLED)
            while((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) != 0);
        #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            SW2_ClearInterrupt();
            SW2_Interrupt_ClearPending();
            SW2_Interrupt_Start();
            CySysPmHibernate();
        }
        else
        {
            /* Device is now in Advertising state */
            DBG_PRINTF("Advertisement is enabled \r\n");
        }
        break;

    case CYBLE_EVT_GAP_DEVICE_CONNECTED:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_DEVICE_CONNECTED: %d \r\n", connHandle.bdHandle);

        /* Send authorization request. */
        (void) CyBle_GapAuthReq(connHandle.bdHandle, &cyBle_authInfo);
        break;

    case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_DEVICE_DISCONNECTED\r\n");
        
        /* Put the device to discoverable mode so that remote can search it. */
        apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        if(apiResult != CYBLE_ERROR_OK)
        {
            DBG_PRINTF("StartAdvertisement API Error: %x \r\n", apiResult);
        }
        break;

    case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_ENCRYPT_CHANGE: %x \r\n", *(uint8 *) eventParam);
        break;

    case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE: %x \r\n", *(uint8 *) eventParam);
        break;

    case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT\r\n");
        break;

#ifdef CYBLE_M0S8BLESS_VERSION_2
    case CYBLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO: security=0x%x, bonding=0x%x, ekeySize=0x%x, err=0x%x \r\n",
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).security,
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).bonding,
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).ekeySize,
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).authErr);
        break;
#endif /* CYBLE_M0S8BLESS_VERSION_2 */

    /**********************************************************
    *                       GATT Events
    ***********************************************************/
    case CYBLE_EVT_GATT_CONNECT_IND:
        connHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GATT_CONNECT_IND: %x \r\n", connHandle.attId);
        break;

    case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
        break;

    case CYBLE_EVT_GATT_DISCONNECT_IND:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GATT_DISCONNECT_IND: \r\n");
        connHandle.attId = 0;
        break;

    case CYBLE_EVT_GATTC_ERROR_RSP:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GATTC_ERROR_RSP: opCode: %x,  handle: %x,  error code: %x \r\n",
            ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->opCode,
            ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->attrHandle,
            ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->errorCode);
        break;

    case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("Discovery complete.\r\n");
        DBG_PRINTF("Discovered services: \r\n");
        for(i = 0u; i < CYBLE_SRVI_COUNT; i++)
        {
            DBG_PRINTF("Service with UUID 0x%x has handle range from 0x%x to 0x%x\r\n",
                   cyBle_serverInfo[i].uuid,
                   cyBle_serverInfo[i].range.startHandle,
                   cyBle_serverInfo[i].range.endHandle);
        }
        break;

    /**********************************************************
    *                       Other Events
    ***********************************************************/
    case CYBLE_EVT_PENDING_FLASH_WRITE:
        /* Inform application that flash write is pending. Stack internal data 
        * structures are modified and require to be stored in Flash using 
        * CyBle_StoreBondingData() */
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_PENDING_FLASH_WRITE\r\n");
        break;

    default:
        DBG_PRINTF("OTHER event: %lx \r\n", event);
		break;
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
    if((CyBle_GetState() == CYBLE_STATE_ADVERTISING) || 
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
                /* Put the CPU into the Sleep mode */
                CySysPmSleep();
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
    char8 command;
    uint8 config[CYBLE_CTS_CLIENT_CONFIG_DESCR_SIZE];
    CYBLE_API_RESULT_T apiResult;

    CyGlobalIntEnable;

    /* Start CYBLE component and register generic event handler */
    CyBle_Start(AppCallBack);

    /* Register the event handler for CTS specific events */
    CyBle_CtsRegisterAttrCallback(CtsAppEventHandler);

    /* Register the event handler for RTUS specific events */
    CyBle_RtusRegisterAttrCallback(RtusAppEventHandler);
    
    /* Register the event handler for NDCS specific events */
    CyBle_NdcsRegisterAttrCallback(NdcsAppEventHandler);

    UART_DEB_Start();

    SW2_Interrupt_StartEx(&ButtonPressInt);

    WDT_Start();

    while(1)
    {
        /* CyBle_ProcessEvents() allows BLE stack to process pending events */
        CyBle_ProcessEvents();

        /* To achieve low power in the device */
        LowPowerImplementation();

        if((command = UART_DEB_UartGetChar()) != 0u)
        {
            switch(command)
            {
            case 'd':              /* Disconnect request */
                apiResult = CyBle_GapDisconnect(connHandle.bdHandle);

                DBG_PRINTF("\r\n");
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("Disconnect Device routine Error: %x.\r\n", apiResult);
                }
                else
                {
                    DBG_PRINTF("Disconnect Device routine Success.\r\n");
                }
                break;

            case '1':     /* Set Notification enable bit in Client Characteristic Configuration
                          * Descriptor of Current Time Characteristic.
                          */
                config[0u] = 0x01u;
                config[1u] = 0x00u;

                apiResult = CyBle_CtscSetCharacteristicDescriptor(connHandle,
                                                                  CYBLE_CTS_CURRENT_TIME,
                                                                  CYBLE_CTS_CURRENT_TIME_CCCD,
                                                                  CYBLE_CCCD_LEN,
                                                                  config);
                DBG_PRINTF("\r\n");
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_CtscSetCharacteristicDescriptor() routine Error: %d \r\n", apiResult);
                }
                else
                {
                    DBG_PRINTF("CyBle_CtscSetCharacteristicDescriptor() routine Success \r\n");
                }
                break;

            case 'o':     /* Display Current Time */
                PrintCurrentTime();
                break;

            case 'i':  /* Information about available commands */
                DBG_PRINTF("\r\n");
                DBG_PRINTF("Available commands:\r\n");
                DBG_PRINTF(" \'d\' - Send disconnect request to peer device.\r\n");
                DBG_PRINTF(" \'1\' - Enable notifications for Current Time Characteristic.\r\n");
                DBG_PRINTF(" \'o\' - Display local device's current time.\r\n");
                break;
            }
        }

        if(TRUE == rtcIntOccured)
        {
            TimeUpdate();
            rtcIntOccured = FALSE;
        }

        HandleLeds();

        if(CYBLE_STATE_CONNECTED == CyBle_GetState())
        {
            if((BUTTON_IS_PRESSED == buttonState) && (FALSE == buttonIsInUse))
            {
                apiResult = CyBle_CtscGetCharacteristicValue(connHandle, CYBLE_CTS_CURRENT_TIME);

                DBG_PRINTF("\r\n");
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_CtscGetCharacteristicValue() routine Error: %d \r\n", apiResult);
                }
                else
                {
                    DBG_PRINTF("CyBle_CtscGetCharacteristicValue() routine Success \r\n");
                }
                buttonIsInUse = TRUE;
            }
        }
        
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


/*******************************************************************************
* Function Name: SW_Interrupt
********************************************************************************
*
* Summary:
*   Handles the mechanical button press.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
CY_ISR(ButtonPressInt)
{
    buttonState = BUTTON_IS_PRESSED;
    SW2_ClearInterrupt();
}


/*******************************************************************************
* Function Name: WDT_Start
********************************************************************************
*
* Summary:
*  Configures and starts Watchdog timer to trigger an interrupt every second.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void WDT_Start(void)
{
    /* Unlock the WDT registers for modification */
    CySysWdtUnlock();
    /* Setup ISR callback */
    WDT_Interrupt_StartEx(&RTC_Interrupt);
    /* Write the mode to generate interrupt on match */
    CySysWdtWriteMode(WDT_COUNTER, CY_SYS_WDT_MODE_INT);
    /* Configure the WDT counter clear on a match setting */
    CySysWdtWriteClearOnMatch(WDT_COUNTER, WDT_COUNTER_ENABLE);
    /* Configure the WDT counter match comparison value */
    CySysWdtWriteMatch(WDT_COUNTER, WDT_COUNT_PERIOD);
    /* Reset WDT counter */
    CySysWdtResetCounters(WDT_COUNTER);
    /* Enable the specified WDT counter */
    CySysWdtEnable(WDT_COUNTER_MASK);
    /* Lock out configuration changes to the Watchdog timer registers */
    CySysWdtLock();
}


/*******************************************************************************
* Function Name: RTC_Interrupt
********************************************************************************
*
* Summary:
*  Interrupt handler for RTC. RTC interrupt occurs every second.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
CY_ISR(RTC_Interrupt)
{
    rtcIntOccured = TRUE;
    buttonIsInUse = FALSE;
    advLedState ^= LED_OFF;
    /* Clear button state */
    buttonState = BUTTON_IS_NOT_PRESSED;
    /* Clear interrupt request */
    CySysWdtClearInterrupt(WDT_INTERRUPT_SOURCE);
}


/*******************************************************************************
* Function Name: HandleLeds
********************************************************************************
*
* Summary:
*  Handles indications on Advertising and Disconnection LEDs.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void HandleLeds(void)
{
    /* If in disconnected state ... */
    if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
    {
        /* ... turn on disconnect indication LED and turn off advertising LED. */
        Disc_Red_LED_Write(LED_ON);
        Adv_Green_LED_Write(LED_OFF);
        ConAct_Blue_LED_Write(LED_OFF);
    }
    /* In advertising state ... */
    else if(CYBLE_STATE_ADVERTISING == CyBle_GetState())
    {
        /* ... turn off disconnect indication and ... */
        Disc_Red_LED_Write(LED_OFF);
        ConAct_Blue_LED_Write(LED_OFF);

        /* ... blink advertising indication LED. */
        Adv_Green_LED_Write(advLedState);
    }
    /* In connected State ... */
    else if(CYBLE_STATE_CONNECTED == CyBle_GetState())
    {
        /* ... turn off disconnect indication and advertising indication LEDs. */
        Disc_Red_LED_Write(LED_OFF);
        Adv_Green_LED_Write(LED_OFF);
        ConAct_Blue_LED_Write(LED_ON);
    }
    else
    {
        /* In this state the LEDs remain in their previous state */
    }
}


/* [] END OF FILE */
