/*******************************************************************************
* File Name: main.c
*
* Version 1.0
*
* Description:
*  This project demonstrates the operation of the CGM Profile
*  in the GATT Server (GAP Peripheral) role.
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

CYBLE_API_RESULT_T apiResult;
uint16 i;
AUTHENTICATION_STATE    authState = AUTHENTICATION_NOT_CONNECTED;
uint32  passkeyValue;
uint32  passkeyPow;
uint8   passkeyCounter;
char8   command;
uint8   customCommand = 0u;
CYBLE_GAP_AUTH_INFO_T   peerAuthInfo;

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
    CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T *connParameters;
    
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
            peerAuthInfo = *(CYBLE_GAP_AUTH_INFO_T *)eventParam;
            DBG_PRINTF("EVT_AUTH_REQ: security=%x, bonding=%x, ekeySize=%x, err=%x \r\n", 
            peerAuthInfo.security, 
            peerAuthInfo.bonding, 
            peerAuthInfo.ekeySize, 
            peerAuthInfo.authErr);
            break;
            
	    case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            DBG_PRINTF("CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST\r\n");
            DBG_PRINTF("Enter the passkey shown in the peer device: \r\n");
            authState = AUTHENTICATION_WAITING_FOR_PASSKEY;
            passkeyValue = 0u;
            passkeyPow = 100000u;
            passkeyCounter = 0u;
            break;
            
	    case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            DBG_PRINTF("Enter this passkey in your peer device: %6.6ld \r\n", *(uint32 *)eventParam);
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
            authState = AUTHENTICATION_BONDING_COMPLETE;
            break;
        
        case CYBLE_EVT_GAP_NUMERIC_COMPARISON_REQUEST:
            DBG_PRINTF("Compare this passkey with displayed in your peer device and press 'y' or 'n': %6.6ld \r\n", 
                                    *(uint32 *)eventParam);
            authState = AUTHENTICATION_PASSKEY_YES_NO;
            break;
            
        case CYBLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO:
            peerAuthInfo = *(CYBLE_GAP_AUTH_INFO_T *)eventParam;
            DBG_PRINTF("CYBLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO: security:%x, bonding:%x, ekeySize:%x, authErr %x \r\n", 
            peerAuthInfo.security, 
            peerAuthInfo.bonding, 
            peerAuthInfo.ekeySize, 
            peerAuthInfo.authErr);
            break;
        
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_CONNECTED: %x \r\n", cyBle_connHandle.bdHandle);
            
             /* Initiate pairing process */
            if((cyBle_authInfo.security & CYBLE_GAP_SEC_LEVEL_MASK) > CYBLE_GAP_SEC_LEVEL_1)
            {
                apiResult = CyBle_GapAuthReq(cyBle_connHandle.bdHandle, &cyBle_authInfo);
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF(" CyBle_GapAuthReq API Error: ");
                    PrintApiResult();
                }
            }
            else
            {
                authState = AUTHENTICATION_BONDING_COMPLETE;
            }
            
            Disconnect_LED_Write(LED_OFF);
            Advertising_LED_Write(LED_OFF);
            Connected_LED_Write(LED_ON);
            break;

        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            DBG_PRINTF("EVT_GAP_DEVICE_DISCONNECTED, reason: %x \r\n", *(uint8*)eventParam);
            /* Put device to discoverable mode so that remote can search it. */
            StartAdvertisement();
            Disconnect_LED_Write(LED_ON);
            Advertising_LED_Write(LED_OFF);
            Connected_LED_Write(LED_OFF);
            break;
            
        case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:       
            connParameters = (CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T *)eventParam;
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_CONNECTED: connIntv = %.2f ms.\r\n",   
            (float)connParameters->connIntv * CYBLE_CONN_INTRV_TO_MS);
            break;
        
        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP:
            DBG_PRINTF("CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP: %s\r\n", 
            *(uint16 *)eventParam == 0u ? "Accepted" : "Rejected");
            break; 

        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            /* Authentication complete; initiate bonding */
            peerAuthInfo = *(CYBLE_GAP_AUTH_INFO_T *)eventParam;
            DBG_PRINTF("AUTH_COMPLETE: security:%x, bonding:%x, ekeySize:%x, authErr %x \r\n", 
            peerAuthInfo.security, 
            peerAuthInfo.bonding, 
            peerAuthInfo.ekeySize, 
            peerAuthInfo.authErr);
            authState = AUTHENTICATION_COMPLETE_BONDING_REQD;
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
                /* Fast and slow advertising period complete, go to low power  
                 * mode (Hibernate mode) and wait for external
                 * user event to wake up device again */
                DBG_PRINTF("Hibernate \r\n");
                Disconnect_LED_Write(LED_ON);
                Advertising_LED_Write(LED_OFF);
                Connected_LED_Write(LED_OFF);
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
                Connected_LED_Write(LED_OFF);
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
            DBG_PRINTF("other event: 0x%lx \r\n", event);
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
    
    /* Blink LED to indicate that device advertises */
    if(CYBLE_STATE_ADVERTISING == CyBle_GetState())
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
* Function Name: AuthenticationHandler
********************************************************************************
*
* Summary:
*   Handles entering authentication information.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void AuthenticationHandler(void)
{
    switch(authState)
    {
    case AUTHENTICATION_PASSKEY_YES_NO:
        
        if(((command = UART_DEB_GET_CHAR()) != 0u) || (customCommand != 0u))
        {
            if((command == 0u) && (customCommand != 0u))
            {
                command = customCommand;
                customCommand = 0u;
            }
            if(command == 'y')
            {
                DBG_PRINTF("Accept displayed passkey \r\n");
                apiResult = CyBle_GapAuthPassKeyReply(cyBle_connHandle.bdHandle, 0u, CYBLE_GAP_ACCEPT_PASSKEY_REQ);
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_GapAuthPassKeyReply API Error : ");
                    PrintApiResult();
                }
                authState = AUTHENTICATION_PASSKEY_ENTERED;
            }
            else if(command == 'n')
            {
                DBG_PRINTF("Reject displayed passkey \r\n");
                apiResult = CyBle_GapAuthPassKeyReply(cyBle_connHandle.bdHandle, 0u, CYBLE_GAP_REJECT_PASSKEY_REQ);
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_GapAuthPassKeyReply API Error : ");
                    PrintApiResult();
                }
               authState = AUTHENTICATION_PASSKEY_ENTERED;
            }
            else
            {
                DBG_PRINTF("Wrong key entered. Press 'y' or 'n'\r\n");
            }
        }
        break;
           
    case AUTHENTICATION_WAITING_FOR_PASSKEY:
     
        /* Get 6 digit number from UART terminal */
        if((command = UART_DEB_GET_CHAR()) != 0u)
        {   
            /* accept the digits that are in the range of '0' to '9' */
            if((command >= '0') && (command <= '9'))  
            {
                passkeyValue += (uint32)(command - '0') * passkeyPow;
                passkeyPow /= 10u;
                passkeyCounter++;
                UART_DEB_PUT_CHAR(command); 
            }
            else  /* If entered digit is not in the range of '0' to '9' */
            {
               DBG_PRINTF("Wrong key entered. \r\n");
            }

            if(passkeyCounter == CYBLE_GAP_USER_PASSKEY_SIZE)
            {
                DBG_PRINTF("\r\n");
                /* Send Pass key Response to create an Authenticated Link */
                apiResult = CyBle_GapAuthPassKeyReply(cyBle_connHandle.bdHandle, passkeyValue, CYBLE_GAP_ACCEPT_PASSKEY_REQ);
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_GapAuthPassKeyReply API Error: ");
                    PrintApiResult();
                }
                authState = AUTHENTICATION_PASSKEY_ENTERED;
            }
        }
        break;

    case AUTHENTICATION_BONDING_COMPLETE:
        break;

    default:    /* Not supported state */
        break;
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
	UART_DEB_Start();
    DBG_PRINTF("\r\nContinuous Glucose Monitoring Sensor Example Project\r\n");
    
    Disconnect_LED_Write(LED_OFF);
    Advertising_LED_Write(LED_OFF);
    Connected_LED_Write(LED_OFF);

    /* Start CYBLE component and register generic event handler */
    apiResult = CyBle_Start(AppCallBack);
	if(apiResult != CYBLE_ERROR_OK)
	{
		DBG_PRINTF("CyBle_Start API Error: ");
        PrintApiResult();
	}
    
    PrintStackVersion();
    
    /* Services initialization */
    CgmsInit();
    BmsInit();
    
    /* Register Timer_Interrupt() by the WDT COUNTER2 to generate interrupt every second */
    CySysWdtSetInterruptCallback(CY_SYS_WDT_COUNTER2, Timer_Interrupt);
    /* Enable the COUNTER2 ISR handler. */
    CySysWdtEnableCounterIsr(CY_SYS_WDT_COUNTER2);
    
    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    while(1u)
    {
        /*******************************************************************
        *  To achieve low power in the device
        *******************************************************************/
        if(( authState != AUTHENTICATION_PASSKEY_YES_NO)
        && ( authState != AUTHENTICATION_WAITING_FOR_PASSKEY) )
        {   
            LowPowerImplementation();
        }
        
        /*******************************************************************
        *  Process all pending BLE events in the stack.
        *******************************************************************/
        CyBle_ProcessEvents();
      
        /*******************************************************************
        *  Process GATT and service-specific tasks during connected state.
        *******************************************************************/
		if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
        {   
            /* Process CGMS actions when device in connection */
            CgmsProcess();
            
		/*******************************************************************
        *  Authentication procdure handling
        *******************************************************************/
            AuthenticationHandler();
        
          /* Store bonding data to flash only when all debug information has been sent */
        #if(CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)
            if((cyBle_pendingFlashWrite != 0u) &&
            ((UART_DEB_GET_TX_BUFF_SIZE() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u))
            {
                apiResult = CyBle_StoreBondingData(0u);
                DBG_PRINTF("Store bonding data, status: %x \r\n", apiResult);
            }
        #endif /* CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES */    
        }
        else
        {
            /* Process BMS actions when device is disconnected */
			BmsProcess();
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
    uint16 i;
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
