/*******************************************************************************
* File Name: main.c
*
* Version: 1.40
*
* Description:
*  BLE HID keyboard example project that supports both input and output reports
*  in boot and protocol mode. The example also demonstrates handling suspend 
*  event from the central device and enters low power mode when suspended.
*  This functionality is implemented based on APIs that are in bootloader 
*  project and shared with bootloadable.
*
* References:
*  BLUETOOTH SPECIFICATION Version 4.1
*  HID Usage Tables spec ver 1.12
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation.  All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include "main.h"

volatile uint32 mainTimer = 0u;

/*******************************************************************************
* Function Name: main()
********************************************************************************
*
* Summary:
*   This is main() function. It configures BLE component and starts it.
*   Also it tracks trigger event (button press) and switches to bootloader
*   application.
*
*******************************************************************************/
int main()
{
    CYBLE_LP_MODE_T lpMode;
    CYBLE_BLESS_STATE_T blessState;
 
#if !defined(__ARMCC_VERSION)
    InitializeBootloaderSRAM();
#endif

    PrintProjectHeader();
   
    ConfigureSharedPins();
   
    CyGlobalIntEnable;  

    /* Start CYBLE component and register generic event handler */
    CyBle_Start(AppCallBack);
    CyBle_GattsDisableAttribute(cyBle_btss.btServiceHandle); /* fix CDT 243443, disable bootloader service */

    WDT_Start();

    while(1) 
    {           
        if(CyBle_GetState() != CYBLE_STATE_INITIALIZING)
        {
            /* Enter DeepSleep mode between connection intervals */
            lpMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
            CyGlobalIntDisable;
            blessState = CyBle_GetBleSsState();

            if(lpMode == CYBLE_BLESS_DEEPSLEEP) 
            {   
                if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
                {
                    CySysPmDeepSleep();
                }
            }
            else
            {
                if(blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
                {
                    CySysPmSleep();
                }
            }
            CyGlobalIntEnable;
        }
        if((CyBle_GetState() == CYBLE_STATE_CONNECTED) && (suspend != CYBLE_HIDS_CP_SUSPEND))
        {
            if(mainTimer != 0u)
            {
                mainTimer = 0u;                
                
                SimulateBattery();
                CyBle_ProcessEvents();
                if(keyboardSimulation == ENABLED)
                {
                    SimulateKeyboard();
                }
            }
            /* Store bonding data to flash only when all debug information has been sent */
            if(cyBle_pendingFlashWrite != 0u)
            {
                #if (DEBUG_UART_ENABLED == YES)
                    CYBLE_API_RESULT_T apiResult;
                    apiResult = CyBle_StoreBondingData(0u);
                    DBG_PRINTF("Store bonding data, status: %x \r\n", apiResult);
                #else
                    (void)CyBle_StoreBondingData(0u);
                #endif /* (DEBUG_UART_ENABLED == YES) */
            }
        }
        CyBle_ProcessEvents();
        
        BootloaderSwitch();
    }   
}


/*******************************************************************************
* Function Name: AppCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  event - event code
*  eventParam - event parameters
*
*******************************************************************************/
void AppCallBack(uint32 event, void* eventParam)
{
    #if (DEBUG_UART_ENABLED == YES)
        CYBLE_GAP_AUTH_INFO_T *authInfo;
    #endif /* (DEBUG_UART_ENABLED == YES) */
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GAP_BD_ADDR_T localAddr;
    uint8 i;
    
    switch (event)
    {
        /**********************************************************
        *                       General Events
        ***********************************************************/
        case CYBLE_EVT_STACK_ON: /* This event is received when the component is Started */
            /* Enter into discoverable mode so that remote can search it. */
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("StartAdvertisement API Error: %d \r\n", apiResult);
            }
            DBG_PRINTF("Bluetooth On, StartAdvertisement with addr: ");
            localAddr.type = 0u;
            CyBle_GetDeviceAddress(&localAddr);
            for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
            {
                DBG_PRINTF("%2.2x", localAddr.bdAddr[i-1]);
            }
            DBG_PRINTF("\r\n");
            break;
        case CYBLE_EVT_TIMEOUT: 
            break;
        case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred. */
            DBG_PRINTF("CYBLE_EVT_HARDWARE_ERROR \r\n");
            break;
            
        /* This event will be triggered by host stack if BLE stack is busy or not busy.
         *  Parameter corresponding to this event will be the state of BLE stack.
         *  BLE stack busy = CYBLE_STACK_STATE_BUSY,
         *  BLE stack not busy = CYBLE_STACK_STATE_FREE 
         */
        case CYBLE_EVT_STACK_BUSY_STATUS:
            DBG_PRINTF("CYBLE_EVT_STACK_BUSY_STATUS: %x\r\n", *(uint8 *)eventParam);
            break;
        case CYBLE_EVT_HCI_STATUS:
            DBG_PRINTF("CYBLE_EVT_HCI_STATUS: %x \r\n", *(uint8 *)eventParam);
            break;
            
        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        case CYBLE_EVT_GAP_AUTH_REQ:
            DBG_PRINTF("CYBLE_EVT_AUTH_REQ: security=%x, bonding=%x, ekeySize=%x, err=%x \r\n", 
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).security, 
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).bonding, 
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).ekeySize, 
                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).authErr);
            break;
        case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            DBG_PRINTF("CYBLE_EVT_PASSKEY_ENTRY_REQUEST press 'p' to enter passkey \r\n");
            break;
        case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            DBG_PRINTF("CYBLE_EVT_PASSKEY_DISPLAY_REQUEST %6.6ld \r\n", *(uint32 *)eventParam);
            break;
        case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
            DBG_PRINTF("CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT \r\n");
            break;
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            #if (DEBUG_UART_ENABLED == YES)
            authInfo = (CYBLE_GAP_AUTH_INFO_T *)eventParam;
            DBG_PRINTF("AUTH_COMPLETE: security:%x, bonding:%x, ekeySize:%x, authErr %x \r\n", 
                                    authInfo->security, authInfo->bonding, authInfo->ekeySize, authInfo->authErr);
            #endif /* (DEBUG_UART_ENABLED == YES) */
            break;
        case CYBLE_EVT_GAP_AUTH_FAILED:
            DBG_PRINTF("CYBLE_EVT_AUTH_FAILED: %x \r\n", *(uint8 *)eventParam);
            break;
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            DBG_PRINTF("CYBLE_EVT_ADVERTISING, state: %x \r\n", CyBle_GetState());
            if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
            {   
                /* Fast and slow advertising period complete, go to low power  
                 * mode (Hibernate mode) and wait for an external
                 * user event to wake up the device again */
                DBG_PRINTF("Hibernate \r\n");
                Advertising_LED_Write(LED_OFF);
                Disconnect_LED_Write(LED_OFF);
                CapsLock_LED_Write(LED_OFF);
                Bootloader_Service_Activation_ClearInterrupt();
                Wakeup_Interrupt_ClearPending();
                Wakeup_Interrupt_Start();
                CySysPmHibernate();
            }
            break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_CONNECTED \r\n");
            Disconnect_LED_Write(LED_OFF);
            Advertising_LED_Write(LED_OFF);
            break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("StartAdvertisement API Error: %d \r\n", apiResult);
            }
            Disconnect_LED_Write(LED_ON);
            break;
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            { 
                uint16 mtu;
                CyBle_GattGetMtuSize(&mtu);
                DBG_PRINTF("CYBLE_EVT_GATTS_XCNHG_MTU_REQ, final mtu= %d \r\n", mtu);
            }
            break;
        case CYBLE_EVT_GATTS_WRITE_REQ:
            DBG_PRINTF("CYBLE_EVT_GATT_WRITE_REQ: %x = ",((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.attrHandle);
            ShowValue(&((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.value);
            (void)CyBle_GattsWriteRsp(((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->connHandle);
            break;
        case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
            DBG_PRINTF("CYBLE_EVT_GAP_ENCRYPT_CHANGE: %x \r\n", *(uint8 *)eventParam);
            break;
        case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_CONNECTION_UPDATE_COMPLETE: %x \r\n", *(uint8 *)eventParam);
            break;
            
        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CYBLE_EVT_GATT_CONNECT_IND:
            DBG_PRINTF("CYBLE_EVT_GATT_CONNECT_IND: %x, %x \r\n", cyBle_connHandle.attId, cyBle_connHandle.bdHandle);
            /* Register service specific callback functions */
            HidsInit();
            BasInit();
            ScpsInit();
            break;
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            DBG_PRINTF("CYBLE_EVT_GATT_DISCONNECT_IND \r\n");
            break;
            
        /**********************************************************
        *                       Other Events
        ***********************************************************/
        default:
            DBG_PRINTF("OTHER event: %lx \r\n", event);
            break;
    }

}


/* [] END OF FILE */
