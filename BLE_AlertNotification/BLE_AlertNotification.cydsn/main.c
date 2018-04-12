/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This is source code for the BLE Alert Notification Client example project.
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

#include <project.h>
#include <stdio.h>
#include <math.h>
#include "ans.h"
#include "common.h"


/***************************************
*        Global Variables
***************************************/
uint8                state = DISCONNECTED;
uint16               advBlinkDelayCount;
uint8                advLedState = LED_OFF;
uint8                buttonState = 0u;
uint16               supportedCategories;


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
    uint8 i;
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GAP_AUTH_INFO_T *authInfo;
    CYBLE_GAP_BD_ADDR_T localAddr;

    switch(event)
    {
    /**********************************************************
    *                       General Events
    ***********************************************************/
    case CYBLE_EVT_STACK_ON: /* This event received when component is started */

        DBG_PRINTF("Bluetooth On. Start advertisement with addr:");

        localAddr.type = 0u;
        CyBle_GetDeviceAddress(&localAddr);

        for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
        {
            DBG_PRINTF("%2.2x", localAddr.bdAddr[i-1]);
        }
        DBG_PRINTF(".\r\n");
        
        /* Starts advertisement */
        if(CYBLE_ERROR_OK == CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST))
        {
            DBG_PRINTF("Device is entered Limited Discovery mode.\r\n"); 
        }
        break;

    case CYBLE_EVT_TIMEOUT:
        DBG_PRINTF("\r\n");
        /* Possible timeout event parameter values:
        * CYBLE_GAP_ADV_MODE_TO -> GAP limited discoverable mode timeout;
        * CYBLE_GAP_AUTH_TO -> GAP pairing process timeout.
        */
        if(CYBLE_GAP_ADV_MODE_TO == *(uint8 *) eventParam)
        {
            DBG_PRINTF("Advertisement timeout occurred. Advertisement will be disabled.\r\n");
        }
        else
        {
            DBG_PRINTF("Timeout occurred.\r\n");
        }
        break;

    case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred. */
        DBG_PRINTF("\r\n");
        DBG_PRINTF("Hardware Error\r\n");
        break;

    case CYBLE_EVT_HCI_STATUS:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("HCI Error. Error code is %x.\r\n", *(uint8 *) eventParam);
        break;

    /**********************************************************
    *                       GAP Events
    ***********************************************************/
    case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
        break;

    case CYBLE_EVT_GAPC_SCAN_START_STOP:
        break;

    case CYBLE_EVT_GAP_AUTH_REQ:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_AUTH_REQ: security=0x%x, bonding=0x%x, ekeySize=0x%x, err=0x%x \r\n",
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
        DBG_PRINTF("CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST. Passkey is: %6.6ld.\r\n",*(uint32 *)eventParam);
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
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_AUTH_FAILED: %x \r\n", *(uint8 *) eventParam);
        break;

    case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
        DBG_PRINTF("\r\n");

        if(0u == *(uint8 *) eventParam)
        {
            if(ADVERTISING == state)
            {
                DBG_PRINTF("Advertisement is disabled \r\n");
                state = DISCONNECTED;
            }
            else
            {
                DBG_PRINTF("Advertisement is enabled \r\n");
                /* Device is now in Advertising state */
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
        DBG_PRINTF("CYBLE_EVT_GAP_DEVICE_CONNECTED: connIntv = %d ms \r\n", 
                                ((CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T *)eventParam)->connIntv * 5u /4u);

        apiResult = CyBle_GattcStartDiscovery(cyBle_connHandle);

        DBG_PRINTF("Start Discovery \r\n");
        if(apiResult != CYBLE_ERROR_OK)
        {
            DBG_PRINTF("StartDiscovery API Error: %x \r\n", apiResult);
        }
        break;

    case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_DEVICE_DISCONNECTED\r\n");
        state = DISCONNECTED;
        
        /* Enter discoverable mode so that remote Client could find the device. */
        if(CYBLE_ERROR_OK == CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST))
        {
            DBG_PRINTF("Device is entered Limited Discovery mode \r\n");
        }
        break;

    case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GAP_ENCRYPT_CHANGE: %x \r\n", *(uint8 *) eventParam);
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
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GATT_CONNECT_IND: %x, %x \r\n", cyBle_connHandle.attId, cyBle_connHandle.bdHandle);

        state = CONNECTED;

        break;

    case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
        break;

    case CYBLE_EVT_GATT_DISCONNECT_IND:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GATT_DISCONNECT_IND: \r\n");
        break;

    case CYBLE_EVT_GATTC_ERROR_RSP:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("CYBLE_EVT_GATTC_ERROR_RSP: opCode: %x,  handle: %x,  error code: %x \r\n",
            ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->opCode,
            ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->attrHandle,
            ((CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam)->errorCode);

        break;

    case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("Discovery complete.\r\n");
        DBG_PRINTF("Discovered services: \r\n");
        for(i = 0u; i < CYBLE_SRVI_COUNT; i++)
        {
            DBG_PRINTF("Service with UUID 0x%x has handle range from 0x%x to 0x%x\r\n",
                   cyBle_serverInfo[i].uuid,
                   cyBle_serverInfo[i].range.startHandle,
                   cyBle_serverInfo[i].range.endHandle);
            if(cyBle_serverInfo[i].uuid == CYBLE_UUID_ALERT_NOTIFICATION_SERVICE)
            {
                if(cyBle_serverInfo[i].range.startHandle < cyBle_serverInfo[i].range.endHandle)
                {
                    DBG_PRINTF("Peer device supports ANS \r\n");
                }
                else
                {
                    DBG_PRINTF("Peer device doesn't support ANS \r\n");
                }
            }
            
        }

        /* Server is now discovered */
        state = CONNECTED_SERVER_DISCOVERED;
        
        /* Send authentication request to peer device */
        (void) CyBle_GapAuthReq(cyBle_connHandle.bdHandle, &cyBle_authInfo);
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
                /* Put the CPU into the Sleep mode */
                CySysPmSleep();
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
* Function Name: main
********************************************************************************
*
* Summary:
*  Main function.
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
    char8 command;
    uint8 config[2u];
    uint8 charVal[2u];
    CYBLE_API_RESULT_T apiResult;

    CyGlobalIntEnable;

    /* Start CYBLE component and register generic event handler */
    CyBle_Start(AppCallBack);

    /* Register the event handler for ANS specific events */
    CyBle_AnsRegisterAttrCallback(AnsServiceAppEventHandler);

    UART_DEB_Start();
    
    SW2_Interrupt_StartEx(&ButtonPressInt);

    while(1)
    {
        /* CyBle_ProcessEvents() allows BLE stack to process pending events */
        CyBle_ProcessEvents();

        /* To achieve low power in the device */
        LowPowerImplementation();

        if((command = UART_DEB_UartGetChar()) != 0u)
        {
            switch(command)
            {
            case 'c':

                DBG_PRINTF("\r\n");
                DBG_PRINTF("Enter category to turn off corresponding LED (\'e\' - \"Email\", \'m\' - \"Missed call\", \r\n");
                DBG_PRINTF("\'s\' - \"SMS/MMS\"):\n");
                do
                {
                    command = UART_DEB_UartGetChar();
                }
                while(command == 0u);

                if((command == 'e') || (command == 'm') || (command == 's'))
                {
                    DBG_PRINTF("%c\r\n",command);
                    if(command == 'e')
                    {
                        DBG_PRINTF("\"Email\" alert LED is cleared.\r\n");
                        Adv_Green_LED_Write(LED_OFF);
                    }
                    else if(command == 'm')
                    {
                        DBG_PRINTF("\"Missed call\" alert LED is cleared.\r\n");
                        Disc_Red_LED_Write(LED_OFF);
                    }
                    else
                    {
                        DBG_PRINTF("\"SMS/MMS\" alert LED is cleared.\r\n");
                        Sms_Blue_LED_Write(LED_OFF);
                    }
                }
                else
                {
                    DBG_PRINTF(" Wrong digit \r\n");
                }
                break;

            case 'd':                   /* Disconnect */
                apiResult = CyBle_GapDisconnect(cyBle_connHandle.bdHandle);

                DBG_PRINTF("\r\n");
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("Disconnect Device routine Error: %x.\r\n", apiResult);
                }
                else
                {
                    DBG_PRINTF("Disconnect Device routine Success.\r\n");
                }
                break;

            case 'e':
                charVal[ANCP_COMMAND_ID_INDEX] = CYBLE_ANS_IMM_NEW_ALERT_NTF;
                charVal[ANCP_CATEGORY_ID_INDEX] = CYBLE_ANS_CAT_ID_EMAIL;

                apiResult =
                    CyBle_AnscSetCharacteristicValue(cyBle_connHandle, CYBLE_ANS_ALERT_NTF_CONTROL_POINT, 2u, charVal);

                DBG_PRINTF("\r\n");
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicValue(CYBLE_ANS_ALERT_NTF_CONTROL_POINT) routine Error: %d \r\n",
                        apiResult);
                }
                else
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicValue(CYBLE_ANS_ALERT_NTF_CONTROL_POINT) routine Success \r\n");
                }
                break;

            case 'm':
                charVal[ANCP_COMMAND_ID_INDEX] = CYBLE_ANS_IMM_NEW_ALERT_NTF;
                charVal[ANCP_CATEGORY_ID_INDEX] = CYBLE_ANS_CAT_ID_MISSED_CALL;

                apiResult =
                    CyBle_AnscSetCharacteristicValue(cyBle_connHandle, CYBLE_ANS_ALERT_NTF_CONTROL_POINT, 2u, charVal);

                DBG_PRINTF("\r\n");
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicValue(CYBLE_ANS_ALERT_NTF_CONTROL_POINT) routine Error: %d \r\n",
                        apiResult);
                }
                else
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicValue(CYBLE_ANS_ALERT_NTF_CONTROL_POINT) routine Success \r\n");
                }
                break;

            case 's':
                charVal[ANCP_COMMAND_ID_INDEX] = CYBLE_ANS_IMM_NEW_ALERT_NTF;
                charVal[ANCP_CATEGORY_ID_INDEX] = CYBLE_ANS_CAT_ID_SMS_MMS;

                apiResult =
                    CyBle_AnscSetCharacteristicValue(cyBle_connHandle, CYBLE_ANS_ALERT_NTF_CONTROL_POINT, 2u, charVal);

                DBG_PRINTF("\r\n");
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicValue(CYBLE_ANS_ALERT_NTF_CONTROL_POINT) routine Error: %d \r\n",
                        apiResult);
                }
                else
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicValue(CYBLE_ANS_ALERT_NTF_CONTROL_POINT) routine Success \r\n");
                }
                break;

            case 'r':

                apiResult = CyBle_AnscGetCharacteristicValue(cyBle_connHandle, CYBLE_ANS_SUPPORTED_NEW_ALERT_CAT);

                DBG_PRINTF("\r\n");
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_AnscGetCharacteristicValue(CYBLE_ANS_SUPPORTED_NEW_ALERT_CAT) routine Error: %d \r\n",
                        apiResult);
                }
                else
                {
                    DBG_PRINTF("CyBle_AnscGetCharacteristicValue(CYBLE_ANS_SUPPORTED_NEW_ALERT_CAT) routine Success \r\n");
                }
                break;

            case 't':
                charVal[ANCP_COMMAND_ID_INDEX] = CYBLE_ANS_EN_NEW_ALERT_NTF;
                charVal[ANCP_CATEGORY_ID_INDEX] = CYBLE_ANS_CAT_ID_ALL;

                apiResult =
                    CyBle_AnscSetCharacteristicValue(cyBle_connHandle, CYBLE_ANS_ALERT_NTF_CONTROL_POINT, 2u, charVal);

                DBG_PRINTF("\r\n");
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicValue(CYBLE_ANS_ALERT_NTF_CONTROL_POINT) routine Error: %d \r\n",
                        apiResult);
                }
                else
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicValue(CYBLE_ANS_ALERT_NTF_CONTROL_POINT) routine Success \r\n");
                }
                break;

            case 'o':
                charVal[ANCP_COMMAND_ID_INDEX] = CYBLE_ANS_DIS_NEW_ALERT_NTF;
                charVal[ANCP_CATEGORY_ID_INDEX] = CYBLE_ANS_CAT_ID_ALL;

                apiResult =
                    CyBle_AnscSetCharacteristicValue(cyBle_connHandle, CYBLE_ANS_ALERT_NTF_CONTROL_POINT, 2u, charVal);

                DBG_PRINTF("\r\n");
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicValue(CYBLE_ANS_ALERT_NTF_CONTROL_POINT) routine Error: %d \r\n",
                        apiResult);
                }
                else
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicValue(CYBLE_ANS_ALERT_NTF_CONTROL_POINT) routine Success \r\n");
                }
                break;

            case '0':     /* Set Notification enable bit in Client Characteristic Configuration
                          * Descriptor of New Alert Characteristic.
                          */
                config[0u] = 0x01u;
                config[1u] = 0x00u;

                apiResult = CyBle_AnscSetCharacteristicDescriptor(cyBle_connHandle,
                                                     CYBLE_ANS_NEW_ALERT,
                                                     CYBLE_ANS_CCCD,
                                                     CYBLE_CCCD_LEN,
                                                     config);
                DBG_PRINTF("\r\n");
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicDescriptor routine Error: %d \r\n", apiResult);
                }
                else
                {
                    DBG_PRINTF("CyBle_AnscSetCharacteristicDescriptor routine Success \r\n");
                }
                break;

            case '1':     /* Set Notification enable bit in Client Characteristic Configuration
                          * Descriptor of New Alert Characteristic.
                          */

                    config[0u] = 0x00u;
                    config[1u] = 0x00u;

                    apiResult = CyBle_AnscSetCharacteristicDescriptor(cyBle_connHandle,
                                                         CYBLE_ANS_NEW_ALERT,
                                                         CYBLE_ANS_CCCD,
                                                         CYBLE_CCCD_LEN,
                                                         config);
                    DBG_PRINTF("\r\n");
                    if(apiResult != CYBLE_ERROR_OK)
                    {
                        DBG_PRINTF("CyBle_AnscSetCharacteristicDescriptor routine Error: %d \r\n", apiResult);
                    }
                    else
                    {
                        DBG_PRINTF("CyBle_AnscSetCharacteristicDescriptor routine Success \r\n");
                    }
                break;

            case 'i':  /* Information about available commands */
                DBG_PRINTF("\r\n");
                DBG_PRINTF("Available commands:\r\n");
                DBG_PRINTF(" \'p\' - Command to enter 6 digit passkey.\r\n");
                DBG_PRINTF(" \'b\' - Initiate bonding procedure with peer device.\r\n");
                DBG_PRINTF(" \'d\' - Send disconnect request to peer device.\r\n");
                DBG_PRINTF(" \'c\' - Turn off LED of New Alert Category that was notified previously. Alert category is\r\n");
                DBG_PRINTF("     specified by the following commands: \'e\' - \"Email\", \'m\' - \"Missed call\", \r\n");
                DBG_PRINTF("     \'s\' - \"SMS/MMS\".\r\n");
                DBG_PRINTF(" \'e\' - Send request with immediate notification command for New Alert \r\n");
                DBG_PRINTF("     Characteristic with Category ID set to \"Email\".\r\n");
                DBG_PRINTF(" \'m\' - Send request with immediate notification command for New Alert \r\n");
                DBG_PRINTF("     Characteristic with Category ID set to \"Missed Call\".\r\n");
                DBG_PRINTF(" \'s\' - Send request with immediate notification command for New Alert \r\n");
                DBG_PRINTF("     Characteristic with Category ID set to \"SMS/MMS\".\r\n");
                DBG_PRINTF(" \'r\' - Read Supported New Alert Category Characteristic. This command required\r\n");
                DBG_PRINTF("     for Client to configure local supported categories setting prior\r\n");
                DBG_PRINTF("     sending notification request to the Server.\r\n");
                DBG_PRINTF(" \'t\' - Send \"Enable New Alert Notification\" command to Alert Notification Control\r\n");
                DBG_PRINTF("     Point Characteristic. Category ID is set to \"All categories\".\r\n");
                DBG_PRINTF(" \'o\' - Send \"Disable New Alert Notification\" command to Alert Notification Control\r\n");
                DBG_PRINTF("     Point Characteristic. Category ID is set to \"All categories\".\r\n");
                DBG_PRINTF(" \'0\' - Enable notifications for New Alert Characteristic.\r\n");
                DBG_PRINTF(" \'1\' - Disable notifications for New Alert Characteristic.\r\n");
                break;
            }
        }

        if(BUTTON_IS_PRESSED == buttonState)
        {
            /* Start advertisement */
            if(CYBLE_ERROR_OK == CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST))
            {
                DBG_PRINTF("Device is entered Limited Discovery mode \r\n");
            }

            /* Clear button state */
            buttonState = BUTTON_IS_NOT_PRESSED;
        }

        HandleLeds();

    }
}


/*******************************************************************************
* Function Name: SW_Interrupt
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
    buttonState = BUTTON_IS_PRESSED;

    SW2_ClearInterrupt();
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
        Disc_Red_LED_Write(LED_ON);
        Adv_Green_LED_Write(LED_OFF);
        Sms_Blue_LED_Write(LED_OFF);
    }
    /* In advertising state ... */
    else if(ADVERTISING == state)
    {
        /* ... turn off disconnect indication and ... */
        Disc_Red_LED_Write(LED_OFF);
        Sms_Blue_LED_Write(LED_OFF);

        /* ... blink advertising indication LED. */
        if(BLINK_DELAY == advBlinkDelayCount)
        {
            if(LED_OFF == advLedState)
            {
                advLedState = LED_ON;
            }
            else
            {
                advLedState = LED_OFF;
            }
            advBlinkDelayCount = 0u;
        }
        advBlinkDelayCount++;

        Adv_Green_LED_Write(advLedState);
    }
    /* In connected State ... */
    else if(CONNECTED == state)
    {
        /* ... turn off disconnect indication and advertising indication LEDs. */
        Disc_Red_LED_Write(LED_OFF);
        Adv_Green_LED_Write(LED_OFF);
        Sms_Blue_LED_Write(LED_OFF);
    }
    else
    {
        /* In this state the LEDs are controlled by the Alert Notification Service */
    }
}


/*******************************************************************************
* Function Name: MakeWordFromBytePtr
********************************************************************************
*
* Summary:
*  Converts array of two bytes, transferred over the BLE, to 16-bit word.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void MakeWordFromBytePtr(uint8 bytePtr[], uint16 *wordPtr)
{
    *wordPtr = (((uint16) bytePtr[1u]) << 8u) | ((uint16) bytePtr[0u]);
}


/* [] END OF FILE */
