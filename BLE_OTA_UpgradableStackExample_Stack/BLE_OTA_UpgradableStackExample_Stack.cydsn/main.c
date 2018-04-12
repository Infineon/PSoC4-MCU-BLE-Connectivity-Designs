/*******************************************************************************
* File Name: main.c
*
* Version: 1.20
*
* Description:
*  This example shows how to build a Stack only project for a BLE design.
*  This project is required for designs that need Over the Air upgrade
*  capability for both Application and the BLE stack. The Design has a BLE
*  component configured as a "Stack Only". The BLE component initialized with
*  a Bootloader service is used as the transport for the bootloader packets
*  from the host. The project also has a bootloader component that uses the
*  packets received from the host to conduct the bootload process.
*
* References:
*  BLUETOOTH SPECIFICATION Version 4.1
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include "project.h"
#include "main.h"

CYBLE_CONN_HANDLE_T connHandle;

#if defined(__ARMCC_VERSION)
    static unsigned long keepMe __attribute__((used));
    __attribute__ ((section(".bootloaderruntype"), zero_init))
    extern volatile uint32 CyReturnToBootloaddableAddress;
#endif /*__ARMCC_VERSION*/

static void PrintProjectHeader(void);

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Starts BLE component and performs all configuration changes required for
*  BLE and Bootloader components operation.
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
    CYBLE_API_RESULT_T apiResult;
    CYBLE_STACK_LIB_VERSION_T stackVersion;
    
    #if defined(__ARMCC_VERSION)    
        keepMe = Image$$DATA$$ZI$$Limit;
        CyReturnToBootloaddableAddress = 0u;
    #endif /*__ARMCC_VERSION*/  
    
    PrintProjectHeader();
    
    CyGlobalIntEnable;
    
    S_LED_Write(LED_ON);
    
    /* Checks if Self Project Image is updated and Runs for the First time */
    AfterImageUpdate();
    
    /* If application image is present and valid but no metadata flag is set.
     * Assuming it is first launch.
     */
    if ((CYRET_SUCCESS == Loader_ValidateApp(1u)) && \
                                (0u == (uint32) CY_GET_XTND_REG8((volatile uint8 *)APP_UPDATE_FLAG_OFFSET)))
    {
        DBG_PRINT_TEXT("Application is ready to be launched. Doing so.\r\n");
        Bootloadable_SetActiveApplication(1u);
        CySoftwareReset();
    }
    
    apiResult = CyBle_Start(AppCallBack);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINT_TEXT("CyBle_Start API Error: ");
        DBG_PRINT_DEC_TEXT(apiResult, "\r\n");
        if(apiResult == CYBLE_ERROR_MEMORY_ALLOCATION_FAILED)
        {
            /* Receiving this error is expected if system heap size is too small for BLE component to start.
             * Please read BLE component datasheet for more details on BLE component stack export mode
             */
            DBG_PRINT_TEXT("MEMORY_ALLOCATION_FAILED need additional Heap size: 0x");
            DBG_PRINT_HEX_TEXT(CYBLE_STACK_RAM_SIZE, "\r\n");
            DBG_PRINT_TEXT("Execution halted.\r\n");
            while (1u == 1u)
            {
                /* Will stay here */
            }
        }
    }
    else
    {
        apiResult = CyBle_GetStackLibraryVersion(&stackVersion);
        if(apiResult != CYBLE_ERROR_OK)
        {
            DBG_PRINT_TEXT("CyBle_GetStackLibraryVersion API Error: 0x");
            DBG_PRINT_HEX8_TEXT(apiResult, "\r\n");
        }
        else
        {
            DBG_PRINT_TEXT("Stack Version: ");
            DBG_PRINT_DEC_TEXT(stackVersion.majorVersion, ".");
            DBG_PRINT_DEC_TEXT(stackVersion.minorVersion, ".");
            DBG_PRINT_DEC_TEXT(stackVersion.patch,        ".");
            DBG_PRINT_DEC_TEXT(stackVersion.buildNumber,  ", used Heap: ");
            DBG_PRINT_DEC_TEXT(CYBLE_STACK_RAM_SIZE, "\r\n");
        }
    }
    
    /* Start Bootloader component in non-blocking mode. */
    Loader_Initialize();
    
    for(;;)
    {
        CyBle_ProcessEvents();
        Loader_HostLink(5u);
        
        /* To have predictable timeout */
        CyDelay(5u);
        
        TimeoutImplementation();
    }
}


/*******************************************************************************
* Function Name: AppCallBack()
********************************************************************************
*
* Summary:
*   This function handles events that are generated by BLE stack.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void AppCallBack(uint32 event, void* eventParam)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GAP_BD_ADDR_T localAddr;
    CYBLE_GAP_CONN_UPDATE_PARAM_T connUpdateParam;
    uint32  i = 0u;

    
    switch (event)
    {
        /**********************************************************
        *                       General Events
        ***********************************************************/
        case CYBLE_EVT_STACK_ON: /* This event received when component is Started */
            /* Enter into discoverable mode so that remote can search it. */
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINT_TEXT("StartAdvertisement API Error: ");
                DBG_PRINT_DEC_TEXT(apiResult, "\r\n");
            }
            DBG_PRINT_TEXT("Bluetooth On, StartAdvertisement with addr: ");
            localAddr.type = 0u;
            CyBle_GetDeviceAddress(&localAddr);
            for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
            {
                DBG_PRINT_HEX_BYTE(localAddr.bdAddr[i-1]);
            }
            DBG_PRINT_TEXT("\r\n");
            break;
        case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred. */
            DBG_PRINT_TEXT("CYBLE_EVT_HARDWARE_ERROR\r\n");
            break;
            

        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        case CYBLE_EVT_GAP_AUTH_REQ:
            DBG_PRINT_TEXT("CYBLE_EVT_AUTH_REQ: security=0x");
                DBG_PRINT_HEX_TEXT((*(CYBLE_GAP_AUTH_INFO_T *)eventParam).security, ", bonding=0x" );
                DBG_PRINT_HEX_TEXT((*(CYBLE_GAP_AUTH_INFO_T *)eventParam).bonding , ", ekeySize=0x");
                DBG_PRINT_HEX_TEXT((*(CYBLE_GAP_AUTH_INFO_T *)eventParam).ekeySize, ", err=0x"     );
                DBG_PRINT_HEX_TEXT((*(CYBLE_GAP_AUTH_INFO_T *)eventParam).authErr , "\r\n"       );
            break;
        case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            DBG_PRINT_TEXT("CYBLE_EVT_PASSKEY_ENTRY_REQUEST press 'p' to enter passkey \r\n");
            break;
        case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            DBG_PRINT_TEXT("CYBLE_EVT_PASSKEY_DISPLAY_REQUEST: ");
            DBG_PRINT_DEC2(*(uint32 *)eventParam, 6);
            DBG_PRINT_TEXT("\r\n");
            break;
        case CYBLE_EVT_GAP_NUMERIC_COMPARISON_REQUEST:
            DBG_PRINT_TEXT("CYBLE_EVT_GAP_NUMERIC_COMPARISON_REQUEST\r\n");
            CyBle_GapAuthPassKeyReply(cyBle_connHandle.bdHandle, 0u, CYBLE_GAP_ACCEPT_PASSKEY_REQ);
            break;
        case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
            DBG_PRINT_TEXT("CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT \r\n");
            break;
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            DBG_PRINT_TEXT("AUTH_COMPLETE \r\n");
            break;
        case CYBLE_EVT_GAP_AUTH_FAILED:
            DBG_PRINT_TEXT("CYBLE_EVT_AUTH_FAILED: 0x");
            DBG_PRINT_HEX8_TEXT( *(uint8 *)eventParam, "\r\n");
            break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            DBG_PRINT_TEXT("CYBLE_EVT_GAP_DEVICE_CONNECTED: ");
            DBG_PRINT_DEC_TEXT(connHandle.bdHandle, "\r\n");
            if ((*(CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T *)eventParam).connIntv > CYBLE_GAPP_CONNECTION_INTERVAL_MAX)
            {
                /* If connection settings do not match expected ones - request parameter update */
                connUpdateParam.connIntvMin = CYBLE_GAPP_CONNECTION_INTERVAL_MIN;
                connUpdateParam.connIntvMax = CYBLE_GAPP_CONNECTION_INTERVAL_MAX;
                connUpdateParam.connLatency = CYBLE_GAPP_CONNECTION_SLAVE_LATENCY;
                connUpdateParam.supervisionTO = CYBLE_GAPP_CONNECTION_TIME_OUT;
                apiResult = CyBle_L2capLeConnectionParamUpdateRequest(cyBle_connHandle.bdHandle, &connUpdateParam);
                DBG_PRINT_TEXT("CyBle_L2capLeConnectionParamUpdateRequest API: 0x");
                DBG_PRINT_HEX8_TEXT(apiResult, "\r\n");
            }
            break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            DBG_PRINT_TEXT("CYBLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINT_TEXT("StartAdvertisement API Error: ");
                DBG_PRINT_DEC_TEXT(apiResult, "\r\n");
            }
            break;
        case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
            DBG_PRINT_TEXT("CYBLE_EVT_GAP_ENCRYPT_CHANGE: 0x");
            DBG_PRINT_HEX8_TEXT( *(uint8 *)eventParam, "\r\n");
            break;
        case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:
            DBG_PRINT_TEXT("CYBLE_EVT_CONNECTION_UPDATE_COMPLETE: 0x");
            DBG_PRINT_HEX8_TEXT( *(uint8 *)eventParam, "\r\n");
            break;
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
            {   
                /* Fast and slow advertising period complete, go to low power  
                 * mode (Hibernate mode) and wait for an external
                 * user event to wake up the device again */
                DBG_PRINT_TEXT("Entering low power mode...\r\n");
                S_LED_Write(1u);
                SW2_S_ClearInterrupt();
                Wakeup_Interrupt_ClearPending();
                Wakeup_Interrupt_Start();
                CySysPmHibernate();
            }
            break;
        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CYBLE_EVT_GATT_CONNECT_IND:
            connHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
            break;
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            connHandle.bdHandle = 0;
            break;
        case CYBLE_EVT_GATTS_PREP_WRITE_REQ:
            (void)CyBle_GattsPrepWriteReqSupport(CYBLE_GATTS_PREP_WRITE_NOT_SUPPORT);
            break;
        case CYBLE_EVT_HCI_STATUS:
            DBG_PRINT_TEXT("CYBLE_EVT_HCI_STATUS\r\n");
        default:
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
    DBG_PRINT_TEXT("> BLE Upgradable Stack Example Stack\r\n");
    DBG_PRINT_TEXT("> Version: 1.20\r\n");
    DBG_PRINT_TEXT("> Compile Date and Time: " __DATE__ " " __TIME__ "\r\n");
    DBG_PRINT_TEXT("\r\n"); 
    DBG_PRINT_TEXT("\r\n");  
    
    return;
}


/* [] END OF FILE */
