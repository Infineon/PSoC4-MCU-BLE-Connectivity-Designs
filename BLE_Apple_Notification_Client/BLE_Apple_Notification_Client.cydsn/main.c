/*******************************************************************************
* File Name: main.c
*
* Version 1.0
*
* Description:
*  This project demonstrates the operation of the Apple Notification Center
*  service in the GATT Client (GAP Peripheral) role.
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


uint8 button = 0u;
volatile uint32 mainTimer;
volatile uint32 ButtonSW2Timer = 0u;
CYBLE_API_RESULT_T apiResult;
uint16 i;
uint8 flag = 0u;
uint16 cccd;


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
            DBG_PRINTF("CYBLE_EVT_STACK_ON\r\n");
			StartAdvertisement();
            break;

        case CYBLE_EVT_STACK_BUSY_STATUS:
            DBG_PRINTF("CYBLE_EVT_STACK_BUSY_STATUS\r\n");
            break;

        case CYBLE_EVT_TIMEOUT: /* 0x01 -> GAP limited discoverable mode timeout. */
                                /* 0x02 -> GAP pairing process timeout. */
                                /* 0x03 -> GATT response timeout. */
            DBG_PRINTF("CYBLE_EVT_TIMEOUT: %d\r\n", *(uint8 *)eventParam);
            break;

        case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred. */
            DBG_PRINTF("CYBLE_EVT_HARDWARE_ERROR\r\n");
            break;

        case CYBLE_EVT_HCI_STATUS:
            DBG_PRINTF("CYBLE_EVT_HCI_STATUS: 0x%2.2x\r\n", *(uint8 *)eventParam);
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
            break;
            
	    case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            DBG_PRINTF("CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST: %6.6ld\r\n", *(uint32 *)eventParam);
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
                    
                case CYBLE_GAP_AUTH_ERROR_LINK_DISCONNECTED:
                    DBG_PRINTF("LINK_DISCONNECTED\r\n");
                    break;
                    
                default:
                    DBG_PRINTF("0x%2.2x \r\n", *(CYBLE_GAP_AUTH_FAILED_REASON_T *)eventParam);
                    break;
            }
            break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_CONNECTED: 0x%2.2x\r\n", cyBle_connHandle.bdHandle);
            ancsFlag = 0u;
            Advertising_LED_Write(LED_OFF);
            apiResult = CyBle_GapAuthReq(cyBle_connHandle.bdHandle, &cyBle_authInfo);
            if(apiResult == CYBLE_ERROR_OK)
        	{
        		DBG_PRINTF("Authentication request is sent\r\n");
        	}
        	else
        	{
        		DBG_PRINTF("CyBle_GapAuthReq API Error: ");
                PrintApiResult();
        	}
            break;

        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            Ringing_LED_Write(LED_OFF);
			StartAdvertisement();
			break;

        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GAP_AUTH_COMPLETE: security:%d, bonding:%d, ekeySize:%d, authErr %d\r\n",
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->security,
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->bonding, 
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->ekeySize, 
                        ((CYBLE_GAP_AUTH_INFO_T *)eventParam)->authErr);
			
	        ancsFlag |= CYBLE_ANCS_FLG_START;
            break;

        case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
            DBG_PRINTF("CYBLE_EVT_GAP_ENCRYPT_CHANGE: %d\r\n", *(uint8 *)eventParam);
            break;
            
        case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
            DBG_PRINTF("CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT\r\n");
            break;
			
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            DBG_PRINTF("CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP\r\n");
            PrintState();
            
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
            {   
                /* Fast and slow advertising period complete, go to low power  
                 * mode (Hibernate mode) and wait for external
                 * user event to wake up device again */
                DBG_PRINTF("Hibernate\r\n");
                Advertising_LED_Write(LED_OFF);
                Disconnect_LED_Write(LED_ON);
                Ringing_LED_Write(LED_OFF);
            #if (DEBUG_UART_ENABLED == ENABLED)
                while((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) != 0);
            #endif /* (DEBUG_UART_ENABLED == ENABLED) */
                SW2_ClearInterrupt();
                Button_Interrupt_ClearPending();
                Button_Interrupt_Start();
                CySysPmHibernate();
            }
            break;
    
    
        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        
        case CYBLE_EVT_GATT_CONNECT_IND:
            DBG_PRINTF("CYBLE_EVT_GATT_CONNECT_IND: attId 0x%2.2x, bdHandle 0x%2.2x\r\n", 
                ((CYBLE_CONN_HANDLE_T *)eventParam)->attId,
                ((CYBLE_CONN_HANDLE_T *)eventParam)->bdHandle);
            break;

        case CYBLE_EVT_GATT_DISCONNECT_IND:
            DBG_PRINTF("CYBLE_EVT_GATT_DISCONNECT_IND\r\n");
            break;
            
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            DBG_PRINTF("CYBLE_EVT_GATTS_XCNHG_MTU_REQ\r\n");
            ancsFlag |= CYBLE_ANCS_FLG_START;
            break;
    
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
                    
                case CYBLE_GATT_WRITE_REQ:
                    DBG_PRINTF("WRITE_REQ");
                    break;
                    
                case CYBLE_GATT_READ_REQ:
                    DBG_PRINTF("READ_REQ");
                    break;
                    
                case CYBLE_GATT_EXECUTE_WRITE_REQ:
                    DBG_PRINTF("EXECUTE_WRITE_REQ");
                    break;

                default:
                    DBG_PRINTF("0x%2.2x", ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->opCode);
                    break;
            }
            DBG_PRINTF(",  handle: 0x%4.4x,  ", ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->attrHandle);
            DBG_PRINTF("errorcode: ");
            switch(((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->errorCode)
            {
                case CYBLE_GATT_ERR_ATTRIBUTE_NOT_FOUND:
                    DBG_PRINTF("ATTRIBUTE_NOT_FOUND");
                    break;

                case CYBLE_GATT_ERR_READ_NOT_PERMITTED:
                    DBG_PRINTF("READ_NOT_PERMITTED");
                    break;
                    
                case CYBLE_GATT_ERR_INSUFFICIENT_AUTHENTICATION:
                    DBG_PRINTF("INSUFFICIENT_AUTHENTICATION");
                    break;
                    
                case CYBLE_GATT_ERR_INSUFFICIENT_ENCRYPTION:
                    DBG_PRINTF("INSUFFICIENT_ENCRYPTION");
                    break;
                    
                case CYBLE_GATT_ERR_INVALID_ATTRIBUTE_LEN:
                    DBG_PRINTF("INVALID_ATTRIBUTE_LEN");
                    break;

                default:
                    DBG_PRINTF("0x%2.2x", ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->errorCode);
                    break;
            }
            DBG_PRINTF("\r\n");
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
            DBG_PRINTF("bdHandle: 0x%2.2x\r\n", ((CYBLE_CONN_HANDLE_T *)eventParam)->bdHandle);
            break;

        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
            DBG_PRINTF("CYBLE_EVT_GATTC_XCHNG_MTU_RSP\r\n");
            break;

        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            DBG_PRINTF("CYBLE_EVT_GATTC_HANDLE_VALUE_NTF\r\n");
            break;
            
        case CYBLE_EVT_GATTC_HANDLE_VALUE_IND:
            DBG_PRINTF("CYBLE_EVT_GATTC_HANDLE_VALUE_IND\r\n");
            break;
            
        case CYBLE_EVT_GATTC_EXEC_WRITE_RSP:
            DBG_PRINTF("CYBLE_EVT_GATTC_EXEC_WRITE_RSP\r\n");
            break;
            
        /**********************************************************
        *                       Discovery Events 
        ***********************************************************/
        case CYBLE_EVT_GATTC_SRVC_DUPLICATION:
            DBG_PRINTF("CYBLE_EVT_GATTC_SRVC_DUPLICATION, UUID: 0x%4.4x\r\n", *(uint16 *)eventParam);
            break;
            
        case CYBLE_EVT_GATTC_CHAR_DUPLICATION:
            DBG_PRINTF("CYBLE_EVT_GATTC_CHAR_DUPLICATION, UUID: 0x%4.4x\r\n", *(uint16 *)eventParam);
            break;
        
        case CYBLE_EVT_GATTC_DESCR_DUPLICATION:
            DBG_PRINTF("CYBLE_EVT_GATTC_DESCR_DUPLICATION, UUID: 0x%4.4x\r\n", *(uint16 *)eventParam);
            break;
            
        case CYBLE_EVT_GATTC_SRVC_DISCOVERY_FAILED:
            DBG_PRINTF("CYBLE_EVT_GATTC_DISCOVERY_FAILED\r\n");
            break;
            
        case CYBLE_EVT_GATTC_SRVC_DISCOVERY_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GATTC_SRVC_DISCOVERY_COMPLETE\r\n");
            break;
            
        case CYBLE_EVT_GATTC_INCL_DISCOVERY_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GATTC_INCL_DISCOVERY_COMPLETE\r\n");
            break;
            
        case CYBLE_EVT_GATTC_CHAR_DISCOVERY_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GATTC_CHAR_DISCOVERY_COMPLETE\r\n");
            break;
            
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GATTC_DISCOVERY_COMPLETE\r\n");
            /* Enable the NS notification */
            cccd = CYBLE_CCCD_NOTIFICATION;
            apiResult = CyBle_AncscSetCharacteristicDescriptor(cyBle_connHandle, CYBLE_ANCS_NS, CYBLE_ANCS_CCCD, CYBLE_CCCD_LEN, (uint8*)&cccd);
        	if(apiResult != CYBLE_ERROR_OK)
        	{
        		DBG_PRINTF("CyBle_AncscSetCharacteristicDescriptor API Error: ");
                PrintApiResult();
        	}
            else
            {
                AncsPrintCharName(CYBLE_ANCS_NS);
                DBG_PRINTF("CCCD write request: 0x%2.2x\r\n", cccd);
            }
            break;
   

        /**********************************************************
        *                  GATT Service Events 
        ***********************************************************/
            
        case CYBLE_EVT_GATTS_INDICATION_ENABLED:
            DBG_PRINTF("CYBLE_EVT_GATTS_INDICATION_ENABLED\r\n");
            break;
        
        case CYBLE_EVT_GATTS_INDICATION_DISABLED:
            DBG_PRINTF("CYBLE_EVT_GATTS_INDICATION_DISABLED\r\n");
            break;
            
        case CYBLE_EVT_GATTC_INDICATION:
            DBG_PRINTF("CYBLE_EVT_GATTS_INDICATION\r\n");
            break;
    
        /**********************************************************
        *                       L2CAP Events 
        ***********************************************************/    
            
        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ:
            DBG_PRINTF("CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ\r\n");
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
            DBG_PRINTF("other event: 0x%lx\r\n", event);
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
    /* Indicate that timer is raised to main loop */
    mainTimer++;
    
    /* Increment timer for recognision 1 pres / 2 pres per second */
    if (ButtonSW2Timer != TIMER_STOP)
    {   
        ButtonSW2Timer ++;
    }
    
    if (ButtonSW2Timer > SW2_POLLING_TIME)
    {
        /* Set flag for button polling */
        flag |= TIMER_SW2;
        
        ButtonSW2Timer = TIMER_STOP;
    }
}

/*******************************************************************************
* Function Name: Button_Interrupt
********************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the SW2 Button.
*
*******************************************************************************/
CY_ISR(Button_SW2_isr)
{
    SW2_ClearInterrupt(); 
    
    if((ancsFlag & CYBLE_ANCS_FLG_ACT) != 0u)
    {
        if (ButtonSW2Timer == TIMER_STOP)
        {   
            ButtonSW2Timer = TIMER_START;
            button = SW2_ONE_PRESSING;
        } 
        
        if (ButtonSW2Timer > SW2_DEBOUNCING_TIME)
        {   
            button = SW2_TWO_PRESSING;
        }
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
* times, the function tries to enter the CPU sleep.
*
*******************************************************************************/
static void LowPowerImplementation(void)
{
    CYBLE_LP_MODE_T bleMode;
    uint8 interruptStatus;
    
    /* For advertising and connected states, implement deep sleep 
     * functionality to achieve low power in system. For more details
     * on low-power implementation, refer to Low Power Application 
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
        else /* When BLE subsystem has been put into sleep mode or is active */
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
*  The function initializes all the components.
*  And then performs all the project functionality in a loop.
*
*******************************************************************************/
int main()
{
    uint8 blinkLed = LED_OFF;
        
    CyGlobalIntEnable;
    UART_DEB_Start();               /* Start communication component */
    DBG_PRINTF("\r\nApple Notification Client Example Project\r\n");
    
    Disconnect_LED_Write(LED_OFF);
    Advertising_LED_Write(LED_OFF);
    Ringing_LED_Write(LED_OFF);
    
    /* Start CYBLE component and register generic event handler */
    apiResult = CyBle_Start(AppCallBack);
	if(apiResult != CYBLE_ERROR_OK)
	{
		DBG_PRINTF("CyBle_Start API Error: ");
        PrintApiResult();
	}
    
    PrintStackVersion();
    
    /* Service initialization */
    AncsInit();
    
    /* Register Timer_Interrupt() by WDT COUNTER2 to generate interrupt every second */
    CySysWdtSetInterruptCallback(CY_SYS_WDT_COUNTER2, Timer_Interrupt);
    /* Enable COUNTER2 ISR handler. */
    CySysWdtEnableCounterIsr(CY_SYS_WDT_COUNTER2);
    
    /* Enable the Interrupt component connected to SW2 */
    Button_Interrupt_StartEx(Button_SW2_isr);
    
    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    while(1u)
    {
        /*******************************************************************
        *  Process all pending BLE events in the stack
        *******************************************************************/
        CyBle_ProcessEvents();
        
        /***********************************************************************
        * Wait for connection established with Central device
        ***********************************************************************/
        if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
        {   
        	/*******************************************************************
            *  Process ANCS tasks
            *******************************************************************/
            AncsProcess();
            
            /* Store bonding data to flash only when all debug information has been sent */
        #if (DEBUG_UART_ENABLED == ENABLED)
            if((cyBle_pendingFlashWrite != 0u) &&
               ((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u))
        #else
            if(cyBle_pendingFlashWrite != 0u)
        #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            {
                apiResult = CyBle_StoreBondingData(0u);
                DBG_PRINTF("Store bonding data, status: ");
				PrintApiResult();
            }
			
            /*******************************************************************
            *  Passkey Entry
            *******************************************************************/
			if((flag & PASSKEY) != 0u)
	        {
	            char8 command;
	            uint32 passkey = 0u;
                uint32 pow10 = 100000ul;;
            
	            flag &= (uint8) ~PASSKEY;
	            DBG_PRINTF("Enter 6 digit passkey: \n");
	                
                for(i = 0u; i < CYBLE_GAP_USER_PASSKEY_SIZE; i++)
                {
                    while((command = UART_DEB_UartGetChar()) == 0); /* Polling next char */
                    
                    /* Check char */
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
                
                /* Send passkey */
                apiResult = CyBle_GapAuthPassKeyReply(cyBle_connHandle.bdHandle, passkey, 1u);
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
        
        /* To achieve low power in device */
        LowPowerImplementation();
        
        /*******************************************************************
        *  Blinking LEDs
        *******************************************************************/
        if(mainTimer > LED_BLINK_RATE)
        {
            mainTimer = 0u;
            blinkLed ^= LED_OFF; /* Toggle blinkLed variable to opposite state */
            
            if(CyBle_GetState() == CYBLE_STATE_ADVERTISING) /* If it is "Advertising" state */
            {
                Advertising_LED_Write(blinkLed); /* Then "Advertising" LED is blinking */
            }
            else if((CyBle_GetState() == CYBLE_STATE_CONNECTED) &&  /* Otherwise, if it is "Connected" state */
                ((ancsFlag & CYBLE_ANCS_FLG_ACT) != 0u)) /* And there is pending user action during incoming call ringing */
            { 
                Ringing_LED_Write(blinkLed); /* Then "Ringing" LED is blinking */
            }
        }
    }
}


/*******************************************************************************
* Function Name: StartDiscovery
********************************************************************************
*
* Summary:
*   Initiates the discovery procedure.
*
* Parameters:
*   None.
*
* Return:
*   None.
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
        DBG_PRINTF("Start Discovery\r\n");
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
    apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("CyBle_GappStartAdvertisement API Error: ");
        PrintApiResult();
    }
    else
    {
        CYBLE_GAP_BD_ADDR_T localAddr;
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
