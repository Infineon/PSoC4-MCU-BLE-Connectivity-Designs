/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This is the source code for the CYBLE Weight Scale example project.
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

#include "project.h"
#include "common.h"
#include "bcs.h"
#include "uds.h"
#include "wss.h"


/***************************************
*        Global Variables
***************************************/
CYBLE_CONN_HANDLE_T         connectionHandle;
uint8                       advLedState = LED_OFF;
uint8                       isButtonPressed = NO;
uint8                       dataLength;
uint8                       userIndex = 0u;
uint16                      justWakeFromDeepSleep = 0u;

/* Used to provide a delay for a button press to cut false pressings */
uint16                      btnPressDelayTimer = BUTTON_PRESS_DELAY;

/* FW timer that defines period of weight scale sensor update simulation */
uint16                      wssSensorUpdateTimer = WSS_SENSOR_TIMER_PERIOD;


/*******************************************************************************
* Function Name: AppCallBack
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the CYBLE Component.
*
* Parameters:  
*  event -       An event from the CYBLE component.
*  eventParams - A structure instance for a corresponding event type. The 
*                list of event structures is available in the component
*                datasheet.
*
* Return: 
*  None.
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
        /* CYBLE_GAP_ADV_MODE_TO - The advertisement time set by the application has expired. */
    	/* CYBLE_GAP_AUTH_TO - An authentication procedure timeout. */
        /* CYBLE_GAP_SCAN_TO - The scan time set by the application has expired. */
        /* CYBLE_GATT_RSP_TO - A GATT procedure timeout. */
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
        DBG_PRINTF("CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST. Passkey is: %6.6ld.\r\n", *(uint32 *)eventParam);
        DBG_PRINTF("Please enter the passkey on your Server device.\r\n");
        break;
    case CYBLE_EVT_GAP_AUTH_COMPLETE:
        authInfo = (CYBLE_GAP_AUTH_INFO_T *)eventParam;

        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_AUTH_COMPLETE: security: 0x%x, bonding: 0x%x, ekeySize: 0x%x, authErr 0x%x \r\n", 
                                authInfo->security, authInfo->bonding, authInfo->ekeySize, authInfo->authErr);
        DBG_PRINTF("Authentication complete.\r\n");
        break;
    case CYBLE_EVT_GAP_AUTH_FAILED:
        DBG_PRINTF("CYBLE_EVT_AUTH_FAILED: %x \r\n", *(uint8 *) eventParam);
        break;
    case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
        DBG_PRINTF("CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP, state: %x\r\n", CyBle_GetState());
        if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
        {   
            /* Fast and slow advertising periods are complete, go to low-power  
             * mode (hibernate mode) and wait for external
             * user event to wake up device again */
            DBG_PRINTF("Hibernate \r\n");
            Disconnect_LED_Write(LED_ON);
            Advertising_LED_Write(LED_OFF);
        #if (DEBUG_UART_ENABLED == ENABLED)
            while((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) != 0);
        #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            SW2_ClearInterrupt();
            Wakeup_Interrupt_ClearPending();
            /* Configure button interrupt */
            Wakeup_Interrupt_StartEx(&ButtonPressInt);
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
        DBG_PRINTF("CYBLE_EVT_GATT_DISCONNECT_IND: \r\n");
        connectionHandle.attId = 0u;
        break;
    case CYBLE_EVT_GATTS_INDICATION_ENABLED:
        break;
    case CYBLE_EVT_GATTS_WRITE_REQ:
        DBG_PRINTF("CYBLE_EVT_GATTS_WRITE_REQ:\r\n");
        break;
    case CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
        /* Triggered on server side when Client sends read request and when
        * characteristic has CYBLE_GATT_DB_ATTR_CHAR_VAL_RD_EVENT property set.
        * This event could be ignored by application unless it need to respond
        * with error response which needs to be set in 'gattErrorCode' field of
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
        DBG_PRINTF("Other event - %lx \r\n", event);
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
    /* Blink green LED to indicate that device advertises */
    if(CyBle_GetState() == CYBLE_STATE_ADVERTISING)
    {
        advLedState ^= LED_OFF;
    }

    wssSensorUpdateTimer--;

    if(btnPressDelayTimer != 0u)
    {
        btnPressDelayTimer--;
    }
}


/*******************************************************************************
* Function Name: ButtonPressInt
********************************************************************************
*
* Summary:
*  Handles the mechanical button press.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
CY_ISR(ButtonPressInt)
{   
    if(btnPressDelayTimer == 0u)
    {
        isButtonPressed = YES;
        /* Set button press delay timer. Currently 1 second. */
        btnPressDelayTimer = BUTTON_PRESS_DELAY;
    }

    SW2_ClearInterrupt();
}


/*******************************************************************************
* Function Name: LowPowerImplementation
********************************************************************************
*
* Summary:
*  Implements low-power mode in the project.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  The function tries to enter deep-sleep whenever the BLE is idle and the UART
*  transmission/reception is not happening. At all other times, the function
*  tries to enter CPU sleep.
*
*******************************************************************************/
static void LowPowerImplementation(void)
{
    CYBLE_LP_MODE_T bleMode;
    uint8 interruptStatus;
    
    /* For scanning and connected states, implement deep-sleep 
     * functionality to achieve low power in system.
     */
    if(CyBle_GetState() != CYBLE_STATE_INITIALIZING)
    {
        /* Request BLE subsystem to enter into deep-sleep mode between connection and advertising intervals */
        bleMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
        /* Disable global interrupts */
        interruptStatus = CyEnterCriticalSection();
        /* When BLE subsystem has been put into deep-sleep mode */
        if(bleMode == CYBLE_BLESS_DEEPSLEEP)
        {
            /* And it is still there or ECO is on */
            if((CyBle_GetBleSsState() == CYBLE_BLESS_STATE_ECO_ON) || 
               (CyBle_GetBleSsState() == CYBLE_BLESS_STATE_DEEPSLEEP))
            {
                /* Put CPU into sleep mode and let SCB to continue sending debug data and receive commands */
                CySysPmSleep();
            }
        }
        else /* When BLE subsystem has been put into sleep mode or is active */
        {
            /* And hardware hasn't finished Tx/Rx operation - put CPU into sleep mode */
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
* Function Name: main
********************************************************************************
*
* Summary:
*  The main function of the project. The function performs the following 
*  actions:
*   1. Performs initialization of the BLE and ISR components. It also resisters
*      all the service's callback functions for the bLE component.
*   2. Invokes handling of the BLE related events.
*   3. Simulates the Weight Scale device operation.
*   4. Performs handling of the low-power modes for the device.
*
*******************************************************************************/
int main()
{
    CYBLE_API_RESULT_T apiResult;
    
    CyGlobalIntEnable;

    /* Initialize the components */
    InitializeWeightScale();
    
    while(1)
    {
        /* CyBle_ProcessEvents() allows BLE stack to process pending events */
        CyBle_ProcessEvents();

        /* Handle advertising and disconnection LED indication */
        HandleLeds();

        /* Handles all the functionality of Weight Scale */
        WeightScaleProfileHandler();
    
    #if(CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)
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
    #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */

        /* Put the device into LPM until the next connection interval event */
        LowPowerImplementation();
    }
}


/*******************************************************************************
* Function Name: SimulateWeightMeasurement
********************************************************************************
*
* Summary:
*  Simulates measurements from the weight scale sensor.
*
*******************************************************************************/
void SimulateWeightMeasurement(void)
{
    /* Check if new height or weight are entered by user */
    if(isUserWeightReceived == YES)
    {
        weightMeasurement[userIndex].weightKg = udsUserRecord[userIndex].weight;
        bodyMeasurement[userIndex].weightKg = udsUserRecord[userIndex].weight;
    }
    if(isUserHeightReceived == YES)
    {
        weightMeasurement[userIndex].heightM = udsUserRecord[userIndex].height;
        bodyMeasurement[userIndex].heightM = udsUserRecord[userIndex].height;
    }
    
    if(weightMeasurement[userIndex].weightKg < SI_MAX_WEIGHT)
    {
        weightMeasurement[userIndex].weightKg = weightMeasurement[userIndex].weightKg + 100u;
    }
    else
    {
        weightMeasurement[userIndex].weightKg = SI_MIN_WEIGHT;
    }
    
    bodyMeasurement[userIndex].weightKg = weightMeasurement[userIndex].weightKg;

    /* Calculate Body Mass index */
    weightMeasurement[userIndex].bmi = (((weightMeasurement[userIndex].weightKg * SI_WEIGHT_RESOLUTION * 
                                       PERCENT_MODIFIER) /
                                       ((weightMeasurement[userIndex].heightM * weightMeasurement[userIndex].heightM) /
                                       (CM_TO_M_DIVIDER * CM_TO_M_DIVIDER * HEIGHT_METER_MODIFIER))) /
                                       (HEIGHT_METER_MODIFIER)) /
                                       (SI_WEIGHT_RESOLUTION_DIVIDER/10u);

    /* Set flag that there is a data for indication */
    isWssIndicationPending = YES;
    isBcsIndicationPending = YES;

    DBG_PRINTF("New measurements from weight sensor\r\n");
    DBG_PRINTF("    New weight: %d.%2.2d kg", CONVERT_TO_KILOGRAMS_INT(weightMeasurement[userIndex].weightKg),
                                                   CONVERT_TO_KILOGRAMS_REM(weightMeasurement[userIndex].weightKg));
    DBG_PRINTF(" BMI: %d.%2.1d %% \r\n", (weightMeasurement[userIndex].bmi / 10u), 
                                                   (weightMeasurement[userIndex].bmi % 10u));

    /* Update weight in UDS */
    UdsSetWeight(weightMeasurement[userIndex].weightKg);
}


/*******************************************************************************
* Function Name: InitializeWeightScale
********************************************************************************
*
* Summary:
*  Performs initialization of BLE and other components utilized in the example.
*
*******************************************************************************/
void InitializeWeightScale(void)
{
    UART_DEB_Start();

    /* Configure button interrupt */
    Wakeup_Interrupt_StartEx(&ButtonPressInt);

    /* Start CYBLE component and register generic event handler */
    CyBle_Start(AppCallBack);

    /* Initialize BLE services */
    BcsInit();
    WssInit();
    UdsInit();

    /* Register Timer_Interrupt() by WDT COUNTER2 to generate interrupt every second */
    CySysWdtSetInterruptCallback(CY_SYS_WDT_COUNTER2, Timer_Interrupt);
    /* Enable COUNTER2 ISR handler. */
    CySysWdtEnableCounterIsr(CY_SYS_WDT_COUNTER2);
}


/*******************************************************************************
* Function Name: WeightScaleProfileHandler
********************************************************************************
*
* Summary:
*  Implements the functionality of the Weight Scale profile.
*
*******************************************************************************/
void WeightScaleProfileHandler(void)
{
    uint8 length;
    CYBLE_API_RESULT_T apiResult;
    uint8 wssIndData[WSS_WS_MEASUREMENT_MAX_DATA_SIZE];
    
    /* Check if it is time to update the weigh scale simulation data */
    if(wssSensorUpdateTimer == 0u)
    {
        wssSensorUpdateTimer = WSS_SENSOR_TIMER_PERIOD;

        if(userIndex != UDS_UNKNOWN_USER)
        {
            SimulateWeightMeasurement();
        }
        else
        {
            DBG_PRINTF("No users registered. Register a new user to start getting weight measurements.\r\n");
        }

        /* Update advertisement data */
        CyBle_WssSetAdUserId(1u, &userIndex);
    }

    /* In connection state, check if there is data that should
    * be sent to remote Client.
    */
    if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
    {
        if(isButtonPressed == YES)
        {
            if((udsRegisteredUserCount - 1u) > userIndex)
            {
                userIndex++;
            }
            else
            {
                userIndex = 0u;
            }
            isButtonPressed = NO;
            UdsLoadUserDataToDb(userIndex);
            SW2_ClearInterrupt();
            Wakeup_Interrupt_ClearPending();
            udsAccessDenied = YES;
            DBG_PRINTF("User changed to %s, %s (User Index: %d) \r\n", udsUserRecord[userIndex].firstName, 
                                                                       udsUserRecord[userIndex].lastName,
                                                                       userIndex);
        }

        /* Handling WSS indications */
        if((isWssIndicationEnabled == YES) && (isWssIndicationPending == YES))
        {
            length = WSS_WS_MEASUREMENT_MAX_DATA_SIZE;

            if(WssPackIndicationData(wssIndData, &length, &weightMeasurement[userIndex]) == WSS_RET_SUCCESS)
            {
                if(udsAccessDenied != YES)
                {
                    apiResult =
                        CyBle_WsssSendIndication(connectionHandle, CYBLE_WSS_WEIGHT_MEASUREMENT, length, wssIndData);
                
                    if(apiResult != CYBLE_ERROR_OK)
                    {
                        DBG_PRINTF("CyBle_WsssSendIndication() API Error: %x \r\n", apiResult);
                    }
                }
                else
                {
                    DBG_PRINTF("WSS indication wasn't sent. Please, provide correct consent.\r\n");
                }
            }
            else
            {
                DBG_PRINTF("WSS data packing failed\r\n");
            }
            isWssIndicationPending = NO;
        }
        else if(isWssIndicationPending == YES)
        {
            DBG_PRINTF("WSS indication wasn't sent. Indications are disabled.\r\n");
            isWssIndicationPending = NO;
        }
        else
        {
            /* No action */
        }

        CyBle_ProcessEvents();

        while(CyBle_IsStackIdle() == CYBLE_ERROR_STACK_BUSY)
        {
            /* Wait for BLE Stack to comple current operation */
            CyBle_ProcessEvents();
        }
        
        /* Handling BCS indications */
        if((isBcsIndicationEnabled == YES) && (isBcsIndicationPending == YES))
        {
            length = BCS_BC_MEASUREMENT_MAX_DATA_SIZE;

            if(BcsPackIndicationData(bcsIndData, &length, &bodyMeasurement[userIndex]) == BCS_RET_SUCCESS)
            {
                if(udsAccessDenied != YES)
                {
                    apiResult = CyBle_BcssSendIndication(connectionHandle, 
                                                         CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT,
                                                         length,
                                                         bcsIndData);

                    if(apiResult != CYBLE_ERROR_OK)
                    {
                        DBG_PRINTF("CyBle_BcssSendIndication() API Error: %x \r\n", apiResult);
                    }
                }
                else
                {
                    DBG_PRINTF("BCS indication wasn't sent. Please, provide correct consent.\r\n");
                }
            }
            else
            {
                DBG_PRINTF("BCS Data packing failed\r\n");
            }
            isBcsIndicationPending = NO;
        }
        else if(isBcsIndicationPending == YES)
        {
            DBG_PRINTF("BCS indication wasn't sent. Indications are disabled.\r\n");
            isBcsIndicationPending = NO;
        }
        else
        {
            /* No action */
        }

        /* Handling UDS indications */
        if((isUdsIndicationPending == YES) && (isUdsIndicationEnabled == YES))
        {
            apiResult = CyBle_UdssSendIndication(connectionHandle, CYBLE_UDS_UCP, udsIndDataSize, ucpResp);
        
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("CyBle_UdssSendIndication() API Error: %x \r\n", apiResult);
            }
            isUdsIndicationPending = NO;
        }

        /* Handling UDS notifications */
        if((isUdsNotificationPending == YES) && (isUdsNotificationEnabled == YES))
        {
            apiResult = CyBle_UdssSendNotification(connectionHandle, 
                CYBLE_UDS_DCI, UDS_NOTIFICATION_SIZE, (uint8 *) &udsUserRecord[userIndex].dbChangeIncrement);
        
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("CyBle_UdssSendNotification() API Error: %x \r\n", apiResult);
            }
            isUdsNotificationPending = NO;
        }
    }
    else if(CyBle_GetState() != CYBLE_STATE_ADVERTISING)
    {
        if(isButtonPressed == YES)
        {
            /* Enter discoverable mode so that remote Client could find the device. */
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("StartAdvertisement API Error: %d \r\n", apiResult);
            }
            isButtonPressed = NO;
            SW2_ClearInterrupt();
            Wakeup_Interrupt_ClearPending();
        }
    }
    else
    {
        /* No action */
    }
}


/*******************************************************************************
* Function Name: HandleLeds
********************************************************************************
*
* Summary:
*  Handles indications on Advertising and Disconnection LEDs.
*
*******************************************************************************/
void HandleLeds(void)
{
    if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
    {
        /* If in disconnected state, turn on disconnect indication LED and turn
        * off Advertising LED.
        */
        Disconnect_LED_Write(LED_ON);
        Advertising_LED_Write(LED_OFF);
    }
    else if(CyBle_GetState() == CYBLE_STATE_ADVERTISING)
    {
        /* In advertising state, turn off disconnect indication LED */
        Disconnect_LED_Write(LED_OFF);
        
        /* Blink advertising indication LED. */
        Advertising_LED_Write(advLedState);
    }
    else
    {
        /* In connected state, turn off disconnect indication and advertising 
        * indication LEDs. 
        */
        Disconnect_LED_Write(LED_OFF);
        Advertising_LED_Write(LED_OFF);
    }
}


/* [] END OF FILE */
