/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  Simple BLE example project that demonstrates how to configure and use 
*  Cypress's BLE component APIs and application layer callback. Device 
*  Information service is used as an example to demonstrate configuring 
*  BLE service characteristics in the BLE component.
*
* References:
*  BLUETOOTH SPECIFICATION Version 4.1
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


#define LED_ON                      (0u)
#define LED_OFF                     (1u)


/*******************************************************************************
* Function Name: ShowError()
********************************************************************************
*
* Summary:
*   Shows error condition: Turn On all leds - white color will indicate error.
*
*******************************************************************************/
void ShowError(void)
{
    Disconnect_LED_Write(LED_ON);
    Advertising_LED_Write(LED_ON);
    /* Halt CPU in Debug mode */
    CYASSERT(0u != 0u);
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

    switch (event)
	{
        /**********************************************************
        *                       General Events
        ***********************************************************/
		case CYBLE_EVT_STACK_ON: /* This event is received when component is Started */
            /* Enter into discoverable mode so that remote can search it. */
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            if(apiResult != CYBLE_ERROR_OK)
            {   
                ShowError();
            }
            break;
		case CYBLE_EVT_TIMEOUT:
			break;
		case CYBLE_EVT_HARDWARE_ERROR:  /* This event indicates that some internal HW error has occurred. */
            ShowError();
            break;
        
        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
            {   
                /* Fast and slow advertising period complete, go to low power  
                 * mode (Hibernate mode) and wait for an external
                 * user event to wake up the device again */
                Advertising_LED_Write(LED_OFF);
                Disconnect_LED_Write(LED_ON);
                SW2_ClearInterrupt();
                Wakeup_Interrupt_ClearPending();
                Wakeup_Interrupt_Start();
                CySysPmHibernate();
            }
            break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            Disconnect_LED_Write(LED_OFF);
            Advertising_LED_Write(LED_OFF);
            break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            /* Put the device to discoverable mode so that remote can search it. */
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            if(apiResult != CYBLE_ERROR_OK)
            {
                ShowError();
            }
            break;
        case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:
            break;
            
        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CYBLE_EVT_GATT_CONNECT_IND:
            break;
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            break;

        /**********************************************************
        *                       Other Events
        ***********************************************************/

        default:
            break;
	}
    
    if(eventParam != NULL)
    {
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
                /* Put the CPU into the Deep-Sleep mode */
                CySysPmDeepSleep();
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
* Function Name: DisUpdateFirmWareRevision()
********************************************************************************
* Summary:
* Updates the Firmware Revision characteristic with BLE Stack version.
*
* Parameters:
* None
*
* Return:
* None
*
*******************************************************************************/
void DisUpdateFirmWareRevision(void)
{
    CYBLE_STACK_LIB_VERSION_T stackVersion;
    uint8 fwRev[9u] = "0.0.0.000";
    
    if(CyBle_GetStackLibraryVersion(&stackVersion) == CYBLE_ERROR_OK)
    {
        /* Transform numbers to ASCII string */
        fwRev[0u] = stackVersion.majorVersion + '0'; 
        fwRev[2u] = stackVersion.minorVersion + '0';
        fwRev[4u] = stackVersion.patch + '0';
        fwRev[6u] = (stackVersion.buildNumber / 100u) + '0';
        stackVersion.buildNumber %= 100u; 
        fwRev[7u] = (stackVersion.buildNumber / 10u) + '0';
        fwRev[8u] = (stackVersion.buildNumber % 10u) + '0';
    }
    
    CyBle_DissSetCharacteristicValue(CYBLE_DIS_FIRMWARE_REV, sizeof(fwRev), fwRev);
    
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
    const char8 serialNumber[] = "123456"; 
    
    CyGlobalIntEnable;  
    
    Disconnect_LED_Write(LED_OFF);
    Advertising_LED_Write(LED_OFF);

    CyBle_Start(AppCallBack);
    
    /* Set Serial Number string not initialized in GUI */
    CyBle_DissSetCharacteristicValue(CYBLE_DIS_SERIAL_NUMBER, sizeof(serialNumber), (uint8 *)serialNumber);
    
    DisUpdateFirmWareRevision();
    
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
	}   
}  


/* [] END OF FILE */

