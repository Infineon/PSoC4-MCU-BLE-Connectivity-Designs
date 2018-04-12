/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This is source code for the CYBLE Running Speed and Cadence Sensor example 
*  project.
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
#include "common.h"
#include "rscs.h"


/***************************************
*        Function Prototypes
***************************************/
void HandleLeds(void);
void AppCallBack(uint32 event, void * eventParam);


/***************************************
*        Global Variables
***************************************/
CYBLE_CONN_HANDLE_T  connectionHandle;
uint8                state = DISCONNECTED;
uint16               advBlinkDelayCount;
uint8                advLedState = LED_OFF;
uint16               justWakeFromDeepSleep = 0u;
uint16               profileTimer = WALKING_PROFILE_TIMER_VALUE;
uint16               paceTimer = PACE_TIMER_VALUE;
uint16               notificationTimer = NOTIFICATION_TIMER_VALUE;
volatile uint32      mainTimer = 0u;


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
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GAP_AUTH_INFO_T *authInfo;
    CYBLE_GAP_BD_ADDR_T localAddr;
    uint8 i;
    
    switch(event)
	{
    /**********************************************************
    *                       General Events
    ***********************************************************/
	case CYBLE_EVT_STACK_ON: /* This event received when component is started */
        DBG_PRINTF("Bluetooth On, Start advertisement with addr: ");
        
        localAddr.type = 0u;
        CyBle_GetDeviceAddress(&localAddr);
        
        for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
        {
            DBG_PRINTF("%2.2x", localAddr.bdAddr[i-1]);
        }
        DBG_PRINTF("\r\n");
        
        /* Enter discoverable mode so that remote Client could find the device. */
        apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        
        if(apiResult != CYBLE_ERROR_OK)
        {
            DBG_PRINTF("StartAdvertisement API Error: %d \r\n", apiResult);
        }
		break;
        
	case CYBLE_EVT_TIMEOUT:
        /* Possible timeout event parameter values: */
        /* CYBLE_GAP_ADV_MODE_TO - Advertisement time set by application has expired */
    	/* CYBLE_GAP_AUTH_TO - Authentication procedure timeout */
        /* CYBLE_GAP_SCAN_TO - Scan time set by application has expired */
        /* CYBLE_GATT_RSP_TO - GATT procedure timeout */
        DBG_PRINTF("TimeOut: %x \r\n", *(uint8 *) eventParam);
		break;
	case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred. */
        DBG_PRINTF("Hardware Error \r\n");
		break;
        
    /**********************************************************
    *                       GAP Events
    ***********************************************************/
    case CYBLE_EVT_GAP_AUTH_REQ:
        DBG_PRINTF("CYBLE_EVT_GAP_AUTH_REQ: security: 0x%x, bonding: 0x%x, ekeySize: 0x%x, authErr: 0x%x \r\n", 
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
        DBG_PRINTF("CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST. Passkey is: %d%d.\r\n", 
            HI16(*(uint32 *)eventParam), 
            LO16(*(uint32 *)eventParam));
        DBG_PRINTF("Please enter the passkey on your Server device.\r\n"); 
        break;
    case CYBLE_EVT_GAP_AUTH_COMPLETE:
        authInfo = (CYBLE_GAP_AUTH_INFO_T *)eventParam;
        (void)authInfo;

        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_AUTH_COMPLETE: security: 0x%x, bonding: 0x%x, ekeySize: 0x%x, authErr 0x%x \r\n", 
                                authInfo->security, authInfo->bonding, authInfo->ekeySize, authInfo->authErr);
        DBG_PRINTF("Bonding complete.\r\n");
        break;
    case CYBLE_EVT_GAP_AUTH_FAILED:
        DBG_PRINTF("CYBLE_EVT_AUTH_FAILED: %x \r\n", *(uint8 *) eventParam);
        break;
    case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
        if(0u == *(uint8 *) eventParam)
        {
            if(ADVERTISING == state)
            {
                DBG_PRINTF("Advertisement is disabled \r\n");
                state = DISCONNECTED;

                if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
                {
                    /* Fast and slow advertising periods complete, go to low power  
                     * mode (Hibernate mode) and wait for an external
                     * user event to wake up the device again */
                    DBG_PRINTF("Hibernate \r\n");
                    Disconnect_LED_Write(LED_ON);
                    Advertising_LED_Write(LED_OFF);
                    Running_LED_Write(LED_OFF);
                    SW2_ClearInterrupt();
                    SW2_Interrupt_ClearPending();
                    SW2_Interrupt_Start();
                #if (DEBUG_UART_ENABLED == ENABLED)
                    while((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) != 0);
                #endif /* (DEBUG_UART_ENABLED == ENABLED) */
                    CySysPmHibernate();
                }
            }
            else
            {
                DBG_PRINTF("Advertisement is enabled \r\n");
                /* Device now is in Advertising state */
                state = ADVERTISING;
            }
        }
        else
        {
            /* Error occurred in the BLE Stack */
            if(ADVERTISING == state)
            {
                DBG_PRINTF("Failed to stop advertising \r\n");
            }
            else
            {
                DBG_PRINTF("Failed to start advertising \r\n");
            }
        }
        break;
    case CYBLE_EVT_GAP_DEVICE_CONNECTED:
        DBG_PRINTF("CYBLE_EVT_DEVICE_CONNECTED: %d \r\n", connectionHandle.bdHandle);
        state = CONNECTED;
        break;
    case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        connectionHandle.bdHandle = 0u;
        DBG_PRINTF("CYBLE_EVT_DEVICE_DISCONNECTED\r\n");
        /* Put the device to discoverable mode so that remote can search it. */
        state = CONNECTED;

        apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        if(apiResult != CYBLE_ERROR_OK)
        {
            DBG_PRINTF("StartAdvertisement API Error: %d \r\n", apiResult);
        }
        break;
    case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
        DBG_PRINTF("ENCRYPT_CHANGE: %x \r\n", *(uint8 *) eventParam);
        break;
    case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE: %x \r\n", *(uint8 *) eventParam);
        break;

    case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT\r\n");
        break;

    /**********************************************************
    *                       GATT Events
    ***********************************************************/
    case CYBLE_EVT_GATT_CONNECT_IND:
        connectionHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
        DBG_PRINTF("CYBLE_EVT_GATT_CONNECT_IND: %x \r\n", connectionHandle.attId);
        break;
    case CYBLE_EVT_GATT_DISCONNECT_IND:
        DBG_PRINTF("CYBLE_EVT_GATT_DISCONNECT_IND: \r\n");
        connectionHandle.attId = 0;
        break;
    case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
        DBG_PRINTF("MTU exchange request received\r\n");
        break;
    case CYBLE_EVT_GATTS_INDICATION_ENABLED:
        break;
    case CYBLE_EVT_GATTS_WRITE_REQ:
        DBG_PRINTF("CYBLE_EVT_GATTS_WRITE_REQ:\r\n");
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
    *                       Other Events
    ***********************************************************/
    case CYBLE_EVT_PENDING_FLASH_WRITE:
        /* Inform application that flash write is pending. Stack internal data 
        * structures are modified and require to be stored in Flash using 
        * CyBle_StoreBondingData() */
        DBG_PRINTF("CYBLE_EVT_PENDING_FLASH_WRITE\r\n");
        break;
    default:
        DBG_PRINTF("OTHER event: %lx \r\n", event);
        break;
	}
}


/*******************************************************************************
* Function Name: WDT_Interrupt
********************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the WDT timer.
*
*******************************************************************************/
CY_ISR(WDT_Interrupt)
{
    if(CySysWdtGetInterruptSource() & WDT_INTERRUPT_SOURCE)
    {
        /* Indicate that timer is raised to the main loop */
        mainTimer++;

        /* Clears interrupt request  */
        CySysWdtClearInterrupt(WDT_INTERRUPT_SOURCE);
    }
}


/*******************************************************************************
* Function Name: WDT_Start
********************************************************************************
*
* Summary:
*  Configures WDT to trigger an interrupt every second.
*
*******************************************************************************/
void WDT_Start(void)
{
    /* Unlock the WDT registers for modification */
    CySysWdtUnlock();
    /* Setup ISR */
    WDT_Interrupt_StartEx(&WDT_Interrupt);
    /* Write the mode to generate interrupt on match */
    CySysWdtWriteMode(WDT_COUNTER, CY_SYS_WDT_MODE_INT);
    /* Configure the WDT counter clear on a match setting */
    CySysWdtWriteClearOnMatch(WDT_COUNTER, WDT_COUNTER_ENABLE);
    /* Configure the WDT counter match comparison value */
    CySysWdtWriteMatch(WDT_COUNTER, WDT_500MSEC);
    /* Reset WDT counter */
    CySysWdtResetCounters(WDT_COUNTER);
    /* Enable the specified WDT counter */
    CySysWdtEnable(WDT_COUNTER_MASK);
    /* Lock out configuration changes to the Watchdog timer registers */
    CySysWdtLock();
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
    if(DISCONNECTED == state)
    {
        /* ... turn on disconnect indication LED and turn off advertising LED. */
        Disconnect_LED_Write(LED_ON);
        Advertising_LED_Write(LED_OFF);
        Running_LED_Write(LED_OFF);
    }
    /* In advertising state ... */
    else if(ADVERTISING == state)
    {
        /* ... turn off disconnect indication and ... */
        Disconnect_LED_Write(LED_OFF);
        Running_LED_Write(LED_OFF);
        
        /* ... blink advertising indication LED. */
        
        if(0u != mainTimer)
        {            
            mainTimer = 0u;
            advLedState ^= LED_OFF;    
        }
        
        Advertising_LED_Write(advLedState);
    }
    /* In connected State ... */
    else
    {
        /* ... turn off disconnect indication and advertising indication LEDs. */
        Disconnect_LED_Write(LED_OFF);
        Advertising_LED_Write(LED_OFF);
        
        if(RUNNING == profile)
        {   
            Running_LED_Write(LED_ON);
        }
        else
        {
            Running_LED_Write(LED_OFF);
        }
    }
}


/*******************************************************************************
* Function Name: ButtonPressInt
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
    if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
    {
        if(WALKING == profile)
        {
            /* Update device with running simulation data */
            profile = RUNNING;
            rscMeasurement.flags |= RSC_FLAGS_WALK_RUN_STATUS_MASK;
        }
        else
        {
            /* Update device with walking simulation data */
            profile = WALKING;
            rscMeasurement.flags &= ~RSC_FLAGS_WALK_RUN_STATUS_MASK;
        }

        rscMeasurement.instStridelen = strideLenRanges[profile].min;
        rscMeasurement.instCadence = cadenceRanges[profile].min;
    }

    SW2_ClearInterrupt();
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
                    
                    /* This variable will set only once per connection interval. This is required
                    * to implement SW timers. Connection interval depends on Client settings.
                    */
                    justWakeFromDeepSleep = 1u;
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
    
    /* Start CYBLE component and register generic event handler */
    CyBle_Start(AppCallBack);
    
    /* Register the event handler for RSCS specific events */
    CyBle_RscsRegisterAttrCallback(RscServiceAppEventHandler);
    
    /* Configure button interrupt */
    SW2_Interrupt_StartEx(&ButtonPressInt);
 
#if (DEBUG_UART_ENABLED == ENABLED)
    UART_DEB_Start();
#endif /* (DEBUG_UART_ENABLED == ENABLED) */
    
    /* Global Resources initialization */
    WDT_Start();
    
    InitProfile();
    
    while(1)
    {
        /* CyBle_ProcessEvents() allows BLE stack to process pending events */
        CyBle_ProcessEvents();

        /* To achieve low power in the device */
        LowPowerImplementation();

        /* Handle advertising LED blinking */
        HandleLeds();

        if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
        {
            if(0u != justWakeFromDeepSleep)
            {
                if(0u == notificationTimer)
                {
                    /* Send notification once in 3 seconds in case of 30 ms connection interval */
                    if(rscNotificationState == ENABLED)
                    {
                        HandleRscNotifications();
                    }
                    notificationTimer = NOTIFICATION_TIMER_VALUE;
                }

                notificationTimer--;
                
                if(0u == paceTimer)
                {
                    /* Update walking/running pace once in 10 seconds in case of 30 ms connection
                    * interval.
                    */
                    UpdatePace();
                    paceTimer = PACE_TIMER_VALUE;
                }

                paceTimer--;

                if(0u == profileTimer)
                {
                    /* Simulate walking/running once in a second/half of a second
                    * in case of 30 ms connection interval.
                    */
                    SimulateProfile();
                    
                    if(WALKING == profile)
                    {
                        profileTimer = WALKING_PROFILE_TIMER_VALUE;
                    }
                    else
                    {
                        profileTimer = RUNNING_PROFILE_TIMER_VALUE;
                    }
                }

                profileTimer--;
                
                justWakeFromDeepSleep = 0u;
            }

            /* Send indication if one is pending */
            if((rscIndicationState == ENABLED) && (YES == rscIndicationPending))
            {
                HandleRscIndications();
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
                (void)apiResult;
                DBG_PRINTF("Store bonding data, status: %x \r\n", apiResult);
            }
        }
    }
}


/* [] END OF FILE */
