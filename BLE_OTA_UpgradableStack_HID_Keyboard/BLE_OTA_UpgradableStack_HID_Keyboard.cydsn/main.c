/*******************************************************************************
* File Name: main.c
*
* Version: 1.20
*
* Description:
*  This example shows how to build a bootloadable BLE application with a
*  BLE component configured as a "Profiles only" component. This is required
*  for designs that need Over the Air upgrade capability for both Application
*  and the BLE stack. The BLE "Profiles only" component can be configured using
*  the usual user profiles like normal BLE component. This component has a
*  dependency with a BLE component which is defined as a part of a separate
*  stack project.
*
* References:
*  BLUETOOTH SPECIFICATION Version 4.1
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "project.h"
#include "main.h"

CYBLE_CONN_HANDLE_T connHandle;

static void PrintProjectHeader(void);


/*******************************************************************************
* Function Name: main()
********************************************************************************
*
* Summary:
* The function configures and starts BLE component. Also, it tracks trigger
* event (button press) and switches to bootloader application.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
int main()
{
    /* Bluetooth Device Address (LSB first) and address type 
    *  The public device address shall be created in accordance with section 9.2 ("48-
    *  bit universal LAN MAC addresses") of the IEEE 802-2001 standard (http://standards.
    *  ieee.org/getieee802/download/802-2001.pdf) and using a valid Organizationally
    *  Unique Identifier (OUI) obtained from the IEEE Registration Authority
    *  (see http://standards.ieee.org/regauth/oui/forms/ and sections 9 and 9.1 of the
    *  IEEE 802-2001 specification).    
    */
    CYBLE_LP_MODE_T lpMode;
    CYBLE_BLESS_STATE_T blessState;
    
    CyGlobalIntEnable;  
    
    LowPower_LED_Write(LED_OFF);
    Advertising_LED_Write(LED_OFF);
    
    /* For GCC compiler use separate API to initialize BLE Stack SRAM.
     * This is required for code sharing.
     */
    #if !defined(__ARMCC_VERSION)
        InitializeBootloaderSRAM();
    #endif

    PrintProjectHeader();

    /* Checks if Self Project Image is updated and Runs for the First time */
    AfterImageUpdate();
    
    CyBle_Start(AppCallBack);

    WDT_Start();
    
	while(1) 
    {              
        if(CyBle_GetState() != CYBLE_STATE_INITIALIZING)
        {
            /* Request BLE subsystem to enter into Deep-Sleep mode between connection and advertising intervals */
            lpMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
            /* Disable global interrupts */
            CyGlobalIntDisable;
            /* Get current BLE subsystem state */
            blessState = CyBle_GetBleSsState();

            /* When BLE subsystem has been put into Deep-Sleep mode */
            if(lpMode == CYBLE_BLESS_DEEPSLEEP)
            {
                /* And it is still there or ECO is on */
                if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
                {
                    /* Put the CPU into the Deep-Sleep mode */
                    CySysPmDeepSleep();
                    
                    /* Handle advertising led blinking */
                    HandleLeds();
                }
            }
            else /* When BLE subsystem has been put into Sleep mode or is active */
            {
                if(blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
                {
                    CySysPmSleep();
                }
            }
            /* Enable global interrupt */
            CyGlobalIntEnable;
        }

        if((CyBle_GetState() == CYBLE_STATE_CONNECTED) && (suspend != CYBLE_HIDS_CP_SUSPEND))
        {
            if(mainTimer != 0u)
            {
                mainTimer = 0u;                
            #if (BAS_SIMULATE_ENABLE != 0)
                SimulateBattery();
                CyBle_ProcessEvents();
            #endif /* BAS_SIMULATE_ENABLE != 0 */    
            #if (BAS_MEASURE_ENABLE != 0)
                MeasureBattery();
                CyBle_ProcessEvents();
            #endif /* BAS_MEASURE_ENABLE != 0 */
                if(keyboardSimulation == ENABLED)
                {
                    SimulateKeyboard();
                }
            }
            /* Store bonding data to flash only when all debug information has been sent */
        #if(CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)
            if((cyBle_pendingFlashWrite != 0u))
            {
                #if (DEBUG_UART_ENABLED == YES)
                    CYBLE_API_RESULT_T apiResult;
                    apiResult = CyBle_StoreBondingData(0u);
                    DBG_PRINTF("Store bonding data, status: %x \r\n", apiResult);
                    (void) apiResult;
                #else
                    (void)CyBle_StoreBondingData(0u);
                #endif /* (DEBUG_UART_ENABLED == YES) */
            }
        #endif /* CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES */    
        }
        
        /* Call ProcessEvents API each connection interval */
        CyBle_ProcessEvents();
        
        HandleLeds();
        
        /* If key press event was detected - debounce it and switch to bootloader emulator mode */
        if (SW2_Read() == 0u)
        {
            CyDelay(500u);
            if (SW2_Read() == 0u)
            {
                #if (DEBUG_UART_ENABLED == YES)
                    DBG_PRINTF("Switching to BLE Upgradable Stack Example Stack project.\r\n");
                    CyDelay(500);
                #endif /* (DEBUG_UART_ENABLED == YES) */
                    if(SW2_Read() == 0u)
                    {
                        DBG_PRINT_TEXT("Release SW2 button to proceed...\r\n");
                        while (SW2_Read() == 0u)
                        {
                            /* Wait for button to be released */
                        }
                    }
                
                Bootloadable_SetActiveApplication(0);
                Bootloadable_Load();
                CySoftwareReset();
            }
        }
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
*  event - the event code
*  *eventParam - the event parameters
*
* Return:
*   None
*
*******************************************************************************/
void AppCallBack(uint32 event, void* eventParam)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GAP_BD_ADDR_T localAddr;
    #if (DEBUG_UART_ENABLED == YES)
        CYBLE_GAP_AUTH_INFO_T *authInfo;
    #endif /* (DEBUG_UART_ENABLED == YES) */
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
                DBG_PRINTF("%2.2x", localAddr.bdAddr[i-1u]);
            }
            DBG_PRINTF("\r\n");
            break;
		case CYBLE_EVT_TIMEOUT: 
            break;
		case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred. */
            DBG_PRINTF("CYBLE_EVT_HARDWARE_ERROR \r\n");
			break;
            
    	/* This event will be triggered by host stack if BLE stack changes its state
         *  from BUSY to FREE and vice versa.
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
                (void) authInfo;
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
                Advertising_LED_Write(LED_ON);
                LowPower_LED_Write(LED_ON);
                SW2_ClearInterrupt();
                Wakeup_Interrupt_ClearPending();
                Wakeup_Interrupt_Start();
                CySysPmHibernate();
            }
            break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_CONNECTED \r\n");
            Advertising_LED_Write(LED_OFF);
            break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("StartAdvertisement API Error: %d \r\n", apiResult);
            }
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
            #if (DEBUG_UART_ENABLED == YES)
                ShowValue(&((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.value);
            #endif /* (DEBUG_UART_ENABLED == YES) */
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


/*******************************************************************************
* Function Name: PrintProjectHeader()
********************************************************************************
* Summary:
*  Prints project header to UART.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static void PrintProjectHeader()
{
    DBG_PRINT_TEXT("\r\n");
    DBG_PRINT_TEXT("\r\n");
    DBG_PRINT_TEXT("> BLE Upgradable Stack HID Keyboard\r\n");
    DBG_PRINT_TEXT("> Version: 1.20\r\n");
    DBG_PRINT_TEXT("> Compile Date and Time: " __DATE__ " " __TIME__ "\r\n");
    DBG_PRINT_TEXT("\r\n"); 
    DBG_PRINT_TEXT("\r\n");  
    
    return;
}


/* [] END OF FILE */
