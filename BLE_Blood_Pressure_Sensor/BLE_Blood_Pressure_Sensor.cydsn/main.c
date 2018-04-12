/*******************************************************************************
* File Name: main.c
*
* Version 1.0
*
* Description:
*  Blood Pressure Sensor example project simulates blood pressure records
*  and sends them over the BLE Blood Pressure Service.
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

volatile uint32 mainTimer = 0;
CYBLE_API_RESULT_T apiResult;


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
        localAddr.type = 0u;
        CyBle_GetDeviceAddress(&localAddr);
        for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
        {
            DBG_PRINTF("%2.2x", localAddr.bdAddr[i-1]);
        }
        DBG_PRINTF("\r\n");
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
*******************************************************************************/
void AppCallBack(uint32 event, void* eventParam)
{
    uint16 i;

    switch(event)
    {
        case CYBLE_EVT_STACK_ON:
            DBG_PRINTF("CYBLE_EVT_STACK_ON \r\n");
            LowPower_LED_Write(LED_OFF);
            batteryMeasureNotify = DISABLED;
            /* Put the device to discoverable mode so that remote can search it. */
            StartAdvertisement();
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
            DBG_PRINTF("CYBLE_EVT_HCI_STATUS \r\n");
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
            Advertising_LED_Write(LED_OFF);
            break;

        case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:
            DBG_PRINTF("CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE \r\n");
            break;

        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_DISCONNECTED \r\n");
            LowPower_LED_Write(LED_OFF);
            batteryMeasureNotify = DISABLED;
            /* Put the device to discoverable mode so that remote can search it. */
            StartAdvertisement();
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
            
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            DBG_PRINTF("CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP, state: %x\r\n", CyBle_GetState());
            PrintState();
            
            if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
            {   
                /* Fast and slow advertising period complete, go to low power  
                 * mode (Hibernate mode) and wait for an external
                 * user event to wake up the device again */
                DBG_PRINTF("Hibernate \r\n");
                Advertising_LED_Write(LED_OFF);
                Disconnect_LED_Write(LED_ON);
                LowPower_LED_Write(LED_OFF);
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
            
        case CYBLE_EVT_GATTS_WRITE_REQ:
            DBG_PRINTF("CYBLE_EVT_GATT_WRITE_REQ: %x = ",((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.attrHandle);
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

        case CYBLE_DEBUG_EVT_BLESS_INT:
            break;

        default:
            DBG_PRINTF("OTHER event: %lx \r\n", event);
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
    
    /* Indicate that timer is raised to the main loop */
    mainTimer++;
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
    DBG_PRINTF("BLE Blood Pressure Sensor Example Project \r\n");

    Disconnect_LED_Write(LED_OFF);
    Advertising_LED_Write(LED_OFF);
    LowPower_LED_Write(LED_OFF);

    if(CYBLE_ERROR_OK != (apiResult = CyBle_Start(AppCallBack)))
    {
        DBG_PRINTF("CyBle_Start API Error: ");
        PrintApiResult();
    }
    
    PrintStackVersion();

    BasInit();
    BlsInit();
    
    ADC_Start();
    
    /* Register Timer_Interrupt() by the WDT COUNTER2 to generate interrupt every second */
    CySysWdtSetInterruptCallback(CY_SYS_WDT_COUNTER2, Timer_Interrupt);
    /* Enable the COUNTER2 ISR handler. */
    CySysWdtEnableCounterIsr(CY_SYS_WDT_COUNTER2);
    
    if(CYBLE_ERROR_OK != (apiResult = CyBle_BlssSetCharacteristicValue(CYBLE_BLS_BPF, sizeof(uint16), (uint8*)&feature)))
    {
        DBG_PRINTF("CyBle_BlssSetCharacteristicValue API Error: %x \r\n", apiResult);
    }

    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    while(1)
    {
        /* Process all the generated events. */
        CyBle_ProcessEvents();

        /* To achieve low power in the device */
        LowPowerImplementation();
        
        /***********************************************************************
        * Wait for connection established with Central device
        ***********************************************************************/
        if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
        {
            /*******************************************************************
            *  Periodically measure a battery level and temperature and send 
            *  results to the Client
            *******************************************************************/        
            if(mainTimer != 0u)
            {
                mainTimer = 0u;                
                
                BlsSimulate();
                MeasureBattery();
            }
            
            /* Store bonding data to flash only when all debug information has been sent */
        #if (DEBUG_UART_ENABLED == ENABLED)
            if((cyBle_pendingFlashWrite != 0u) &&
               ((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u))
        #else
            if(cyBle_pendingFlashWrite != 0u)
        #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            {
                CYBLE_API_RESULT_T apiResult;
                
                apiResult = CyBle_StoreBondingData(0u);
                DBG_PRINTF("Store bonding data, status: %x \r\n", apiResult);
            }
        }
    }
}


/* [] END OF FILE */
