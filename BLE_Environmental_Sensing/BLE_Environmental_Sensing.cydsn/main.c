/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This is the source code for the CYBLE Environmental Sensing example 
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

/***************************************
*        Function Prototypes
***************************************/
void HandleLeds(void);
void AppCallBack(uint32 event, void * eventParam);
void WDT_Start(void);
void WDT_Stop(void);

/***************************************
*        Global Variables
***************************************/
CYBLE_CONN_HANDLE_T  connectionHandle;
uint8                advLedState = LED_OFF;
volatile uint32      mainTimer = 0u;
uint32               prevMainTimer;
uint8                isButtonPressed = NO;

/* Contains the value of the Change Index which is advertised in the 
* Service Data AD field. By the spec it should be a pseudo-random value
* but in this project, for simplicity, instead of a pseudo-random value,
* a value incremented from an old value will be used.
*/
uint16  essChangeIndex = 0u;

CYBLE_ESS_CHARACTERISTIC_DATA_T humidity;
CYBLE_ESS_CHARACTERISTIC_DATA_T windSpeed[SIZE_2_BYTES];


/*******************************************************************************
* Function Name: AppCallBack
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the CYBLE Component.
*
* Parameters:  
*  uint8 event:       An event from the CYBLE component.
*  void* eventParams: A structure instance for a corresponding event type. The 
*                     list of event structures is available in the component
*                     datasheet.
*
* Return: 
*  None
*
*******************************************************************************/
void AppCallBack(uint32 event, void *eventParam)
{
    uint32 i;
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GAP_AUTH_INFO_T *authInfo;
    CYBLE_GAP_BD_ADDR_T localAddr;
    
    switch(event)
	{
        /**********************************************************
        *                       General Events
        ***********************************************************/
    	case CYBLE_EVT_STACK_ON: /* This event is received when component is started */
            DBG_PRINTF("Bluetooth On, Start advertisement with addr: ");
            
            localAddr.type = 0u;
            CyBle_GetDeviceAddress(&localAddr);
            
            for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
            {
                DBG_PRINTF("%2.2x", localAddr.bdAddr[i-1]);
            }
            DBG_PRINTF("\r\n");
            
            /* Enter discoverable mode so that remote Client could find device. */
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("StartAdvertisement API Error: %d \r\n", apiResult);
            }
    		break;
            
    	case CYBLE_EVT_TIMEOUT:
            /* Possible timeout event parameter values: */
            /* CYBLE_GAP_ADV_MODE_TO - The advertisement time set by the application has expired */
        	/* CYBLE_GAP_AUTH_TO - An authentication procedure timeout */
            /* CYBLE_GAP_SCAN_TO - Scan time set by the application has expired */
            /* CYBLE_GATT_RSP_TO - A GATT procedure timeout */
            DBG_PRINTF("TimeOut: %x \r\n", *(uint8 *) eventParam);
    		break;
    	case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that an internal HW error has occurred. */
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
            DBG_PRINTF("\r\n");
            DBG_PRINTF("CYBLE_EVT_GAP_AUTH_COMPLETE: security: 0x%x, bonding: 0x%x, ekeySize: 0x%x, authErr 0x%x \r\n", 
                                    authInfo->security, authInfo->bonding, authInfo->ekeySize, authInfo->authErr);
            DBG_PRINTF("Bonding complete.\r\n");
            break;
        case CYBLE_EVT_GAP_AUTH_FAILED:
            DBG_PRINTF("EVT_AUTH_FAILED: %x \r\n", *(uint8 *) eventParam);
            break;
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            DBG_PRINTF("CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP, state: %x\r\n", CyBle_GetState());
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
            {   
                /* Fast and slow advertising period is complete, go to low power  
                 * mode (hibernate mode) and wait for external
                 * user event to wake up device again */
                DBG_PRINTF("Hibernate \r\n");
                Disconnect_LED_Write(LED_ON);
                Advertising_LED_Write(LED_OFF);
                Connected_LED_Write(LED_OFF);
                while((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) != 0);
                SW2_ClearInterrupt();
                Wakeup_Interrupt_ClearPending();
                Wakeup_Interrupt_Start();
                CySysPmHibernate();
            }
            break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            DBG_PRINTF("CYBLE_EVT_DEVICE_CONNECTED: %d \r\n", connectionHandle.bdHandle);
            break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            connectionHandle.bdHandle = 0u;
            DBG_PRINTF("CYBLE_EVT_DEVICE_DISCONNECTED\r\n");

            /* Enter discoverable mode so that remote Client could find device. */
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
            DBG_PRINTF("EVT_GATT_DISCONNECT_IND: \r\n");
            connectionHandle.attId = 0u;
            break;
        case CYBLE_EVT_GATTS_INDICATION_ENABLED:
            break;
        case CYBLE_EVT_GATTS_WRITE_REQ:
            DBG_PRINTF("CYBLE_EVT_GATTS_WRITE_REQ:\r\n");
            break;

        /**********************************************************
        *                       Other Events
        ***********************************************************/
        default:
            DBG_PRINTF("Unknown event - %x \r\n", LO16(event));
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
        /* Indicate that timer is raised to main loop */
        mainTimer++;
        
        if(windSpeed[CHARACTERISTIC_INSTANCE_1].isMeasurementPeriodElapsed == YES)
        {
            windSpeed[CHARACTERISTIC_INSTANCE_1].updateIntervalTimer--;
        }
        if(windSpeed[CHARACTERISTIC_INSTANCE_2].isMeasurementPeriodElapsed == YES)
        {
            windSpeed[CHARACTERISTIC_INSTANCE_2].updateIntervalTimer--;
        }
        if(humidity.isMeasurementPeriodElapsed == YES)
        {
            humidity.updateIntervalTimer--;
        }

        if(windSpeed[CHARACTERISTIC_INSTANCE_1].ntfTimer != 0u)
        {
            windSpeed[CHARACTERISTIC_INSTANCE_1].ntfTimer--;
        }
        if(windSpeed[CHARACTERISTIC_INSTANCE_2].ntfTimer != 0u)
        {   
            windSpeed[CHARACTERISTIC_INSTANCE_2].ntfTimer--;
            
        }
        if(humidity.ntfTimer != 0u)
        {   
            humidity.ntfTimer--;
            
        }
        
        /* Update state of Advertising LED */
        advLedState ^= LED_OFF;
        
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
    /* Unlock WDT registers for modification */
    CySysWdtUnlock();
    /* Set up ISR */
    WDT_Interrupt_StartEx(&WDT_Interrupt);
    /* Write mode to generate interrupt on match */
    CySysWdtWriteMode(WDT_COUNTER, CY_SYS_WDT_MODE_INT);
    /* Configure WDT counter clear on match setting */
    CySysWdtWriteClearOnMatch(WDT_COUNTER, WDT_COUNTER_ENABLE);
    /* Configure WDT counter match comparison value */
    CySysWdtWriteMatch(WDT_COUNTER, WDT_1SEC);
    /* Reset WDT counter */
    CySysWdtResetCounters(WDT_COUNTER);
    /* Enable specified WDT counter */
    CySysWdtEnable(WDT_COUNTER_MASK);
    /* Lock out configuration changes to Watchdog timer registers */
    CySysWdtLock();
}


/*******************************************************************************
* Function Name: WDT_Stop
********************************************************************************
*
* Summary:
*  This API stops the WDT timer.
*
*******************************************************************************/
void WDT_Stop(void)
{
    /* Unlock WDT registers for modification */
    CySysWdtUnlock(); 
    /* Disable specified WDT counter */
    CySysWdtDisable(WDT_COUNTER_MASK);
    /* Locks out configuration changes to Watchdog timer registers */
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
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void HandleLeds(void)
{
    if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
    {
        /* If in disconnected state turn on disconnect indication LED and turn
        * off Advertising LED.
        */
        Disconnect_LED_Write(LED_ON);
        Advertising_LED_Write(LED_OFF);
        Connected_LED_Write(LED_OFF);
    }
    else if(CyBle_GetState() == CYBLE_STATE_ADVERTISING)
    {
        /* In advertising state turn off disconnect indication LED */
        Disconnect_LED_Write(LED_OFF);
        Connected_LED_Write(LED_OFF);
        
        /* Blink advertising indication LED. */
        Advertising_LED_Write(advLedState);
    }
    else
    {
        /* In connected state turn off disconnect indication and advertising 
        * indication LEDs. 
        */
        Disconnect_LED_Write(LED_OFF);
        Advertising_LED_Write(LED_OFF);
        Connected_LED_Write(LED_ON);
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
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
CY_ISR(ButtonPressInt)
{
    /* In connected state - indicate that button was pressed. Check timer
    * value to cut false button presses.
    */
    if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
    {
        isButtonPressed = YES;
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
                /* Put the CPU into the Deep-Sleep mode when all debug information has been sent */
                if((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u)
                {
                    CySysPmDeepSleep();
                }
                else /* Put the CPU into Sleep mode and let SCB to continue sending debug data */
                {
                    CySysPmSleep();
                }
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
    CYBLE_API_RESULT_T apiResult;
    
    CyGlobalIntEnable;
    
    /* Set initial Change Index value */
    CyBle_EsssSetChangeIndex(essChangeIndex);
    
    /* Start CYBLE component and register generic event handler */
    CyBle_Start(AppCallBack);
    
    /* Configure button interrupt */
    Wakeup_Interrupt_StartEx(&ButtonPressInt);

    UART_DEB_Start();

    EssInit();
    
    /* Global Resources initialization */
    WDT_Start();

    while(1)
    {
        /* CyBle_ProcessEvents() allows BLE stack to process pending events */
        CyBle_ProcessEvents();

        /* To achieve low power in the device */
        LowPowerImplementation();

        /* Handle advertising LED blinking */
        HandleLeds();

        /* Check if there is updated data */
        SimulateProfile(&windSpeed[0]);
        SimulateProfile(&windSpeed[1]);
        SimulateProfile(&humidity);

        /* In connection state check if there is data that
        * should be sent to remote Client.
        */
        if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
        {
            if(prevMainTimer != mainTimer)
            {
                if(isButtonPressed == YES)
                {
                    /* Change ES Configuration descriptor value and indicate it to Client */
                    HandleButtonPress(&windSpeed[0]);  
                    HandleButtonPress(&windSpeed[1]);  
                    HandleButtonPress(&humidity);  
                    
                    isIndicationPending = YES;
                    indicationValue = CYBLE_ESS_VALUE_CHANGE_ES_CONFIG;
                    isButtonPressed = NO;
                }

                /* Check if there are notifications for True Wind Speed #1 and send them */
                ChkNtfAndSendData(&windSpeed[0]);

                /* Check if there are notifications for True Wind Speed #2 and send them */
                ChkNtfAndSendData(&windSpeed[1]);                
                
                /* Check if there are notifications for Humidity and send them */
                ChkNtfAndSendData(&humidity);                

                /* Check if there are indications need to send to remote Client
                * and send them.
                */
                if((isIndicationEnabled == YES) && (isIndicationPending == YES))
                {
                    HandleIndication(indicationValue);
                    isIndicationPending = NO;
                    CyBle_ProcessEvents();
                }
                
            }
            prevMainTimer = mainTimer;
        }

        /* Store bonding data to flash only when all debug information has been sent */
        if((cyBle_pendingFlashWrite != 0u) &&
           ((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u))
        {
            apiResult = CyBle_StoreBondingData(0u);
            (void)apiResult;
            DBG_PRINTF("Store bonding data, status: %x \r\n", apiResult);
        }
    }
}


/* [] END OF FILE */
