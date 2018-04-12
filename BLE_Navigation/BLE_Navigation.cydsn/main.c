/*******************************************************************************
* File Name: main.c
*
* Version 1.0
*
* Description:
*  The navigation example project simulates the navigation device (GATT Server) workflow
*  and sends the navigation data over BLE Location and Navigation Service.
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

#include "main.h"

volatile uint32 mainTimer;
CYBLE_API_RESULT_T apiResult;
uint16 i;
uint8 flag = 0u;
uint8 led = LED_OFF;


/*******************************************************************************
* Function Name: AppCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  event - The event code.
*  *eventParam - The event parameters.
*
*******************************************************************************/
void AppCallBack(uint32 event, void* eventParam)
{   
    switch(event)
    {
        case CYBLE_EVT_STACK_ON:
            DBG_PRINTF("CYBLE_EVT_STACK_ON \r\n");
            /* Put device to discoverable mode so that remote can search it. */
            StartAdvertisement();
            Advertising_LED_Write(LED_OFF);
			break;

        case CYBLE_EVT_STACK_BUSY_STATUS:
            DBG_PRINTF("CYBLE_EVT_STACK_BUSY_STATUS \r\n");
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
            DBG_PRINTF("CYBLE_EVT_HCI_STATUS: %x \r\n", *(uint8 *)eventParam);
            break;

        /**********************************************************
        *                       GAP Events
        ***********************************************************/

        case CYBLE_EVT_GAP_AUTH_REQ:
            DBG_PRINTF("CYBLE_EVT_GAP_AUTH_REQ\r\n");
            break;
            
	    case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            DBG_PRINTF("CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST\r\n");
            flag |= PASSKEY;
            CySysWdtResetCounters(CY_SYS_WDT_COUNTER2_MASK);
            mainTimer = 0u;
            break;
            
	    case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            DBG_PRINTF("CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST: %6.6ld\r\n", *(uint32 *)eventParam);
        #if(CYBLE_IO_CAPABILITY == CYBLE_GAP_IOCAP_DISPLAY_YESNO)
            apiResult = CyBle_GapAuthPassKeyReply(cyBle_connHandle.bdHandle, 0u, CYBLE_GAP_ACCEPT_PASSKEY_REQ);
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("CyBle_GapAuthPassKeyReply API Error: ");
				PrintApiResult();
            }
            else
            {
                DBG_PRINTF("Passkey is accepted\r\n");
            }
        #endif
            break;
            
        case CYBLE_EVT_GAP_AUTH_FAILED:
            DBG_PRINTF("CYBLE_EVT_GAP_AUTH_FAILED, reason: ");
            switch(*(CYBLE_GAP_AUTH_FAILED_REASON_T *)eventParam)
            {
                case CYBLE_GAP_AUTH_ERROR_NONE:
                    DBG_PRINTF("NONE\r\n");
                    break;

                case CYBLE_GAP_AUTH_ERROR_PASSKEY_ENTRY_FAILED:
                    DBG_PRINTF("PASSKEY_ENTRY_FAILED\r\n");
                    break;

                case CYBLE_GAP_AUTH_ERROR_OOB_DATA_NOT_AVAILABLE:
                    DBG_PRINTF("OOB_DATA_NOT_AVAILABLE\r\n");
                    break;

                case CYBLE_GAP_AUTH_ERROR_AUTHENTICATION_REQ_NOT_MET:
                    DBG_PRINTF("AUTHENTICATION_REQ_NOT_MET\r\n");
                    break;

                case CYBLE_GAP_AUTH_ERROR_CONFIRM_VALUE_NOT_MATCH:
                    DBG_PRINTF("CONFIRM_VALUE_NOT_MATCH\r\n");
                    break;

                case CYBLE_GAP_AUTH_ERROR_PAIRING_NOT_SUPPORTED:
                    DBG_PRINTF("PAIRING_NOT_SUPPORTED\r\n");
                    break;

                case CYBLE_GAP_AUTH_ERROR_INSUFFICIENT_ENCRYPTION_KEY_SIZE:
                    DBG_PRINTF("INSUFFICIENT_ENCRYPTION_KEY_SIZE\r\n");
                    break;

                case CYBLE_GAP_AUTH_ERROR_COMMAND_NOT_SUPPORTED:
                    DBG_PRINTF("COMMAND_NOT_SUPPORTED\r\n");
                    break;

                case CYBLE_GAP_AUTH_ERROR_UNSPECIFIED_REASON:
                    DBG_PRINTF("UNSPECIFIED_REASON\r\n");
                    break;

                case CYBLE_GAP_AUTH_ERROR_REPEATED_ATTEMPTS:
                    DBG_PRINTF("REPEATED_ATTEMPTS\r\n");
                    break;

                case CYBLE_GAP_AUTH_ERROR_INVALID_PARAMETERS:
                    DBG_PRINTF("INVALID_PARAMETERS\r\n");
                    break;

                case CYBLE_GAP_AUTH_ERROR_AUTHENTICATION_TIMEOUT:
                    DBG_PRINTF("AUTHENTICATION_TIMEOUT\r\n");
                    break;
	              
                case CYBLE_GAP_AUTH_ERROR_LINK_DISCONNECTED:
                    DBG_PRINTF("LINK_DISCONNECTED\r\n");
                    break;
                    
                default:
                    DBG_PRINTF("0x%x\r\n", *(CYBLE_GAP_AUTH_FAILED_REASON_T *)eventParam);
                    break;
            }
            break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_CONNECTED: %x \r\n", cyBle_connHandle.bdHandle);
            Disconnect_LED_Write(LED_OFF);
            Advertising_LED_Write(LED_OFF);
            break;

        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_DISCONNECTED \r\n");
            /* Put device to discoverable mode so that remote can search it. */
            StartAdvertisement();
            Disconnect_LED_Write(LED_ON);
            Advertising_LED_Write(LED_OFF);
            Navigation_LED_Write(LED_OFF);
            break;

        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GAP_AUTH_COMPLETE: security:%x, bonding:%x, ekeySize:%x, authErr %x \r\n",
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->security,
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->bonding, 
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->ekeySize, 
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->authErr);
            break;

        case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
            DBG_PRINTF("CYBLE_EVT_GAP_ENCRYPT_CHANGE: %d \r\n", *(uint8 *)eventParam);
            break;
            
        case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
            DBG_PRINTF("CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT \r\n");
            break;
			
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            DBG_PRINTF("CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP \r\n");
            PrintState();
            
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
            {   
                /* Fast and slow advertising period complete, go to low-power  
                 * mode (Hibernate mode) and wait for external
                 * user event to wake device up again */
                DBG_PRINTF("Hibernate \r\n");
                Disconnect_LED_Write(LED_ON);
                Advertising_LED_Write(LED_OFF);
                Navigation_LED_Write(LED_OFF);
            #if (DEBUG_UART_ENABLED == ENABLED)
                while((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) != 0);
            #endif /* (DEBUG_UART_ENABLED == ENABLED) */
                SW2_ClearInterrupt();
                Wakeup_Interrupt_ClearPending();
                Wakeup_Interrupt_Start();
                CySysPmHibernate();
            }
            else if(CyBle_GetState() == CYBLE_STATE_ADVERTISING)
            {
                Advertising_LED_Write(LED_ON);
                Disconnect_LED_Write(LED_OFF);
            }
            else
            {
                /* Nothing else */   
            }
            break;

        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        
        case CYBLE_EVT_GATT_CONNECT_IND:
            DBG_PRINTF("CYBLE_EVT_GATT_CONNECT_IND: attId %x, bdHandle %x \r\n", 
                ((CYBLE_CONN_HANDLE_T *)eventParam)->attId, ((CYBLE_CONN_HANDLE_T *)eventParam)->bdHandle);
            break;

        case CYBLE_EVT_GATT_DISCONNECT_IND:
            DBG_PRINTF("CYBLE_EVT_GATT_DISCONNECT_IND \r\n");
            break;
    
        case CYBLE_EVT_GATTS_WRITE_REQ:
            DBG_PRINTF("CYBLE_EVT_GATTS_WRITE_REQ attr handle: %4.4x , value: ",((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.attrHandle);
            for(i = 0; i < ((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.value.len; i++)
            {
                DBG_PRINTF("%2.2x ", ((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.value.val[i]);
            }
            DBG_PRINTF("\r\n");
            break;
            
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            DBG_PRINTF("CYBLE_EVT_GATTS_XCNHG_MTU_REQ \r\n");
            break;
            
        case CYBLE_EVT_GATTS_HANDLE_VALUE_CNF:
            DBG_PRINTF("CYBLE_EVT_GATTS_HANDLE_VALUE_CNF \r\n");
            break;
            
        case CYBLE_EVT_GATTS_PREP_WRITE_REQ:
            DBG_PRINTF("CYBLE_EVT_GATTS_PREP_WRITE_REQ \r\n");
            break;
        
        case CYBLE_EVT_GATTS_EXEC_WRITE_REQ:
            DBG_PRINTF("CYBLE_EVT_GATTS_EXEC_WRITE_REQ \r\n");
            break;

        /**********************************************************
        *                  GATT Service Events 
        ***********************************************************/
            
        case CYBLE_EVT_GATTS_INDICATION_ENABLED:
            DBG_PRINTF("CYBLE_EVT_GATTS_INDICATION_ENABLED \r\n");
            break;
        
        case CYBLE_EVT_GATTS_INDICATION_DISABLED:
            DBG_PRINTF("CYBLE_EVT_GATTS_INDICATION_DISABLED \r\n");
            break;
            
        case CYBLE_EVT_GATTC_INDICATION:
            DBG_PRINTF("CYBLE_EVT_GATTS_INDICATION \r\n");
            break;

        case CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            /* Triggered on server side when client sends read request and when
            * characteristic has CYBLE_GATT_DB_ATTR_CHAR_VAL_RD_EVENT property set.
            * This event could be ignored by application unless it need to response
            * by error response which needs to be set in gattErrorCode field of
            * event parameter. */
            DBG_PRINTF("CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ: handle: %x \r\n", 
                ((CYBLE_GATTS_CHAR_VAL_READ_REQ_T *)eventParam)->attrHandle);
            break;

        /**********************************************************
        *                       L2CAP Events 
        ***********************************************************/    
            
        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ:
            DBG_PRINTF("CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ \r\n");
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
            DBG_PRINTF("Other event: 0x%lx \r\n", event);
			break;
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
    
    /* Indicate that timer is raised to main loop */
    mainTimer++;

    /* Blink green LED to indicate that device advertises */
    if(CyBle_GetState() == CYBLE_STATE_ADVERTISING)
    {
        led ^= LED_OFF;
        Advertising_LED_Write(led);
    }
}


/*******************************************************************************
* Function Name: LowPowerImplementation()
********************************************************************************
* Summary:
* Implements low power in the project.
*
* Parameters:
* None.
*
* Return:
* None.
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
     * functionality to achieve low power in system. For more details
     * on low power implementation, refer to Low Power Application 
     * Note.
     */
    if((CyBle_GetState() == CYBLE_STATE_ADVERTISING) || 
       (CyBle_GetState() == CYBLE_STATE_CONNECTED))
    {
        /* Request BLE subsystem to enter into deep sleep mode between connection and advertising intervals */
        bleMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
        /* Disable global interrupts */
        interruptStatus = CyEnterCriticalSection();
        /* When BLE subsystem has been put into deep sleep mode */
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
            /* And hardware doesn't finish Tx/Rx opeation - put CPU into sleep mode */
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
*  The main function for the project.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  The function starts the BLE and UART components.
*  This function processes all BLE events and also implements the low-power 
*  functionality.
*
*******************************************************************************/
int main()
{
    CyGlobalIntEnable;
    UART_DEB_Start();
    DBG_PRINTF("\r\nBLE Location and Navigation Example Project\r\n");
    
    Disconnect_LED_Write(LED_OFF);
    Advertising_LED_Write(LED_OFF);
    Navigation_LED_Write(LED_OFF);

    /* Start CYBLE component and register generic event handler */
    apiResult = CyBle_Start(AppCallBack);
	if(apiResult != CYBLE_ERROR_OK)
	{
		DBG_PRINTF("CyBle_Start API Error: ");
        PrintApiResult();
	}
    
    PrintStackVersion();
    
    /* Services initialization */
    BasInit();
	LnsInit();

    /* Register Timer_Interrupt() by WDT COUNTER2 to generate interrupt every second */
    CySysWdtSetInterruptCallback(CY_SYS_WDT_COUNTER2, Timer_Interrupt);
    /* Enable COUNTER2 ISR handler. */
    CySysWdtEnableCounterIsr(CY_SYS_WDT_COUNTER2);

    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    while(1u)
    {
        /* CyBle_ProcessEvents() allows BLE stack to process pending events */
        CyBle_ProcessEvents();

        /* To achieve low power in device */
        LowPowerImplementation();
        
        /*******************************************************************
        *  Process GATT and service-specific tasks during connected state.
        *******************************************************************/
		if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
        {   
			/*******************************************************************
            *  Process LNS CP requests
            *******************************************************************/
			LnsProcess();

        #if (DEBUG_UART_ENABLED == ENABLED)
            if((cyBle_pendingFlashWrite != 0u) &&
               ((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u))
            
        #else
            if(cyBle_pendingFlashWrite != 0u)
        #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            {
				/* Store bonding data to flash only when all debug information has been sent */
                apiResult = CyBle_StoreBondingData(0u);
                DBG_PRINTF("Store bonding data, status: ");
				PrintApiResult();
            }
            
            if(mainTimer != 0u)
            {
                mainTimer = 0u;                
                
                /*******************************************************************
                *  Periodically simulates a battery level and sends it to the Client.  ?
                *******************************************************************/
                SimulateBattery();
                
                /*******************************************************************
                *  Periodically sends LNS notifications to the Client.		?
                *******************************************************************/
                LnsNtf();
                
				
                /*******************************************************************
                *  Passkey entry
                *******************************************************************/
				if((flag & PASSKEY) != 0u)
		        {
		            char8 command = 0;
		            uint32 passkey = 0u;
	                uint32 pow10 = 100000ul;
            
		            flag &= (uint8) ~PASSKEY;
	                DBG_PRINTF("Enter 6 digit passkey: \n");
	            
	                for(i = 0u; i < CYBLE_GAP_USER_PASSKEY_SIZE; i++)
	                {
	                    while((command = UART_DEB_UartGetChar()) == 0);
                    
	                    if((command >= '0') && (command <= '9'))
	                    {
	                        passkey += (uint32)(command - '0') * pow10;
	                        pow10 /= 10u;
	                        DBG_PRINTF("%c\n", command);
	                    }
	                    else
	                    {
	                        DBG_PRINTF(" Wrong digit\r\n");
	                        break;
	                    }
	                }
                
	                if(i == CYBLE_GAP_USER_PASSKEY_SIZE)
	                {
		                apiResult = CyBle_GapAuthPassKeyReply(cyBle_connHandle.bdHandle, passkey, 1);
		                if(apiResult != CYBLE_ERROR_OK)
		                {
		                    DBG_PRINTF(" CyBle_GapAuthPassKeyReply API Error: ");
		                    PrintApiResult();
		                }
		                else
		                {
		                    DBG_PRINTF(" Passkey is sent\r\n");
		                }
	                }
		        }
            }
        }
    }
}

/*******************************************************************************
* Function Name: StartAdvertisement
********************************************************************************
*
* Summary:
*   Initiates the advertisement procedure.
*   Prints the Device Address.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void StartAdvertisement(void)
{
    CYBLE_GAP_BD_ADDR_T localAddr;
    apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("StartAdvertisement API Error: ");
        PrintApiResult();
    }
    else
    {
        DBG_PRINTF("Start Advertisement with addr: ");
        CyBle_GetDeviceAddress(&localAddr);
        for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
        {
            DBG_PRINTF("%2.2x", localAddr.bdAddr[i-1]);
        }
        DBG_PRINTF("\r\n");
    }
}

/* [] END OF FILE */
