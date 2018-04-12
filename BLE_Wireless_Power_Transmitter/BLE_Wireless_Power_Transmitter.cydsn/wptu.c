/*******************************************************************************
* File Name: wptu.c
*
* Version: 1.0
*
* Description:
*  This file contains WPTS callback handler function and code for Power 
*  Transmitter Unit.
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

#include "common.h"
#include "wptu.h"
#include <stdbool.h>

uint32 powerTimer = 1u;
uint16 powerSimulation;
uint16 powerCPResponce;

extern CYBLE_PEER_DEVICE_INFO_T peedDeviceInfo[CYBLE_MAX_ADV_DEVICES];
extern uint8 deviceN;
extern uint8 customCommand;
extern bool requestResponce;
extern uint8 state;

CYBLE_PTU_STATIC_PAR_T ptuStaticPar;


/*******************************************************************************
* Function Name: WptsCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service specific events from 
*   Wireless Power Transfer Service.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
* Return:
*  None.
*
*******************************************************************************/
void WptsCallBack (uint32 event, void *eventParam)
{
    CYBLE_WPTS_CHAR_VALUE_T *eventPar = (CYBLE_WPTS_CHAR_VALUE_T *)eventParam;
    CYBLE_WPTS_DESCR_VALUE_T *descrValue = (CYBLE_WPTS_DESCR_VALUE_T *)eventParam;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    
    switch(event)
    {
        case CYBLE_EVT_WPTSC_NOTIFICATION:
            DBG_PRINTF("CYBLE_EVT_WPTSC_NOTIFICATION charIndex= #%d Value= ", eventPar->charIndex);
            ShowValue(eventPar->value, 0u);
            break;
        case CYBLE_EVT_WPTSC_INDICATION:
            DBG_PRINTF("CYBLE_EVT_WPTSC_INDICATION charIndex= #%d Value= ", eventPar->charIndex);
            ShowValue(eventPar->value, 0u);
            break;
        case CYBLE_EVT_WPTSC_WRITE_CHAR_RESPONSE:
            requestResponce = true;
            DBG_PRINTF("CYBLE_EVT_WPTSC_WRITE_CHAR_RESPONSE: charIndex =%x \r\n", eventPar->charIndex);
            if((state == STATE_CONNECTED) && (eventPar->charIndex == CYBLE_WPTS_PTU_STATIC_PAR))
            {
                /* Enable Notification */
                customCommand = '1';
            }
            break;
        case CYBLE_EVT_WPTSC_READ_CHAR_RESPONSE:
            requestResponce = true;
            /* Parse static characteristic value */
            if(eventPar->charIndex == CYBLE_WPTS_PRU_STATIC_PAR)
            {
                CYBLE_PRU_STATIC_PAR_T pruStaticPar;
                
                pruStaticPar = *(CYBLE_PRU_STATIC_PAR_T *)(((CYBLE_WPTS_CHAR_VALUE_T *)eventParam)->value->val);
                peedDeviceInfo[deviceN].pruStaticPar = pruStaticPar;
                DBG_PRINTF("PRU_STATIC_PARAMETER: flags: %x, protocol rev: %d, ", 
                    pruStaticPar.flags, pruStaticPar.protocolRev);
                DBG_PRINTF("category: Category %d , ", pruStaticPar.pruCategory);
                DBG_PRINTF("information: %x , ", pruStaticPar.pruInformation);
                
                DBG_PRINTF("hardware rev: %d , firmware rev: %d ", pruStaticPar.hardwareRev, pruStaticPar.firmwareRev);
                DBG_PRINTF("Prect_max: %d mW, ", pruStaticPar.pRectMax * PRU_STATIC_PAR_PREACT_MAX_MULT);
                DBG_PRINTF("Vrect_min_static: %d mV, ", pruStaticPar.vRectMinStatic);
                DBG_PRINTF("Vrect_high_static: %d mV, ", pruStaticPar.vRectHighStatic);
                DBG_PRINTF("Vrect_set: %d mV, ", pruStaticPar.vRectSet);
                if((pruStaticPar.flags & PRU_STATIC_PAR_FLAGS_ENABLE_DELTA_R1) != 0u)
                {
                    DBG_PRINTF("deltaR1: %.2f ohms\r\n", (float)pruStaticPar.deltaR1 * PRU_STATIC_PAR_DELTA_R1_MULT);
                }
                if(state == STATE_CONNECTED)
                {
                    apiResult = CyBle_WptscSetCharacteristicValue(cyBle_connHandle, CYBLE_WPTS_PTU_STATIC_PAR, 
                        sizeof(ptuStaticPar), (uint8 *)&ptuStaticPar);
                    (void)apiResult;
                    DBG_PRINTF("Set PTU Static Parameter char value, apiResult: %x \r\n", apiResult);
                }
            }
            else if(eventPar->charIndex == CYBLE_WPTS_PRU_DYNAMIC_PAR)
            {
                CYBLE_PRU_DYNAMIC_PAR_T pruDynamicPar;
                pruDynamicPar = *(CYBLE_PRU_DYNAMIC_PAR_T *)(((CYBLE_WPTS_CHAR_VALUE_T *)eventParam)->value->val);
                DBG_PRINTF("PRU_DYNAMIC_PARAMETER: flags: %x,", pruDynamicPar.flags);
                DBG_PRINTF("Vrect: %d mV, ", pruDynamicPar.vRect);
                DBG_PRINTF("Irect: %d mA, ", pruDynamicPar.iRect);
                if((pruDynamicPar.flags & PRU_DYNAMIC_PAR_FLAGS_VOUT_EN) != 0u)
                {
                    DBG_PRINTF("Vout: %d mV, ", pruDynamicPar.vOut);
                }
                if((pruDynamicPar.flags & PRU_DYNAMIC_PAR_FLAGS_IOUT_EN) != 0u)
                {
                    DBG_PRINTF("Iout: %d mA, ", pruDynamicPar.iOut);
                }
                if((pruDynamicPar.flags & PRU_DYNAMIC_PAR_FLAGS_TEMPERATURE_EN) != 0u)
                {
                    DBG_PRINTF("Temp: %d C, ", pruDynamicPar.temperature - PRU_DYNAMIC_PAR_TEMPERATURE_OFFSET);
                }
                if((pruDynamicPar.flags & PRU_DYNAMIC_PAR_FLAGS_VREACT_MIN_EN) != 0u)
                {
                    DBG_PRINTF("\r\n  VrectMinDyn: %d mV, ", pruDynamicPar.vRectMinDyn);
                }
                if((pruDynamicPar.flags & PRU_DYNAMIC_PAR_FLAGS_VREACT_SET_EN) != 0u)
                {
                    DBG_PRINTF("VrectSetDyn: %d mV, ", pruDynamicPar.vRectSetDyn);
                }
                if((pruDynamicPar.flags & PRU_DYNAMIC_PAR_FLAGS_VREACT_HIGH_EN) != 0u)
                {
                    DBG_PRINTF("VrectHighDyn: %d mV, ", pruDynamicPar.vRectHighDyn);
                }
                DBG_PRINTF("Alert: %x.\r\n", pruDynamicPar.alert);
                /* Congifuration is done */
                if(state == STATE_CONNECTED) 
                {
                    state = STATE_CONFIGURED;
                }
            }
            else
            {
                DBG_PRINTF("CYBLE_EVT_WPTSC_READ_CHAR_RESPONSE: charIndex =%x, ", eventPar->charIndex);
                DBG_PRINTF(" value_len = %x, value = ", eventPar->value->len);
                ShowValue(eventPar->value, 0u);
            }
            break;
        case CYBLE_EVT_WPTSC_READ_DESCR_RESPONSE:
            requestResponce = true;
            (void)*descrValue;
            DBG_PRINTF("CYBLE_EVT_WPTSC_READ_DESCR_RESPONSE: charIndex =%x, ", descrValue->charIndex);
            DBG_PRINTF(" descrIndex =%x, value_len = %x, value = ", descrValue->descrIndex, descrValue->value->len);
            ShowValue(eventPar->value, 0u);
            break;
        case CYBLE_EVT_WPTSC_WRITE_DESCR_RESPONSE:
            requestResponce = true;
            DBG_PRINTF("CYBLE_EVT_WPTSC_WRITE_DESCR_RESPONSE charIndex =%x \r\n", eventPar->charIndex);
            if((state == STATE_CONNECTED) && (eventPar->charIndex == CYBLE_WPTS_PRU_ALERT))
            {
                /* Enable reading of PRU dynamic characteristic */
                customCommand = '8';
            }
            break;
		default:
            DBG_PRINTF("CPSC OTHER event: %lx \r\n", event);
			break;
    }
}




/*******************************************************************************
* Function Name: WptsInit()
********************************************************************************
*
* Summary:
*   Initializes the WPTS service.
*
*******************************************************************************/
void WptsInit(void)
{
   
    /* Register service specific callback function */
    CyBle_WptsRegisterAttrCallback(WptsCallBack);
    
    (void)memset(&peedDeviceInfo, 0, sizeof(peedDeviceInfo));
    
    /* Init PTU static parameters */
    ptuStaticPar.flags = PTU_STATIC_PAR_FLAGS_MAX_IMPEDANCE_EN | PTU_STATIC_PAR_FLAGS_MAX_RESISTANCE_EN;
    ptuStaticPar.ptuPower = PRU_STATIC_PAR_POWER_DEF;    
    ptuStaticPar.ptuClass = PRU_STATIC_PAR_CLASS_DEF - PTU_STATIC_PAR_CLASS_OFFSET;
    ptuStaticPar.ptuMaxSourceImpedance = PRU_STATIC_PAR_IMPEDANCE_DEF << PTU_STATIC_PAR_MAX_SOURCE_IMPEDANCE_SHIFT;
    ptuStaticPar.ptuMaxLoadResistance = PRU_STATIC_PAR_RESISTANCE_DEF << PTU_STATIC_PAR_MAX_LOAD_RESISTANCE_SHIFT; 
    ptuStaticPar.hardwareRev = PRU_STATIC_PAR_HARDW_REV_DEF;
    ptuStaticPar.firmwareRev = PRU_STATIC_PAR_FW_REV_DEF;
    ptuStaticPar.protocolRev = PRU_STATIC_PAR_PROTOCOL_REV_DEF;
    ptuStaticPar.ptuDevNumber = 1u - PTU_STATIC_PAR_NUMBER_OF_DEVICES_OFFSET;
    
}


/****************************************************************************** 
##Function Name: WptsScanProcessEventHandler
*******************************************************************************

Summary:
 This function handles CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT event for WPTS.

Parameters:
 *eventParam: the pointer to a data structure specified by the event.
 *serviceData: output parameter with service data

Return:
 Non zero value when the advertising packet contains WPTS service data.

******************************************************************************/
uint32 WptsScanProcessEventHandler(CYBLE_GAPC_ADV_REPORT_T *eventParam, CYBLE_PRU_ADV_SERVICE_DATA_T *serviceData)
{
    uint32 servicePresent = 0u; 
    uint32 advIndex = 0u;
    
    do
    {
        /* Find Service Data AD type with WPT service UUID. */
        if((eventParam->data[advIndex] >= PRU_ADV_SERV_DATA_LEN) &&
           (eventParam->data[advIndex + PRU_ADV_SERV_DATA_TYPE_OFFSET] == (uint8)CYBLE_GAP_ADV_SRVC_DATA_16UUID) &&
           (CyBle_Get16ByPtr(&eventParam->data[advIndex + PRU_ADV_SERV_DATA_SERV_OFFSET]) == 
                CYBLE_UUID_WIRELESS_POWER_TRANSFER_SERVICE)
           )
        {
            serviceData->wptsServiceHandle = CyBle_Get16ByPtr(&eventParam->data[advIndex + PRU_ADV_SERV_DATA_HANDLE_OFFSET]);
            serviceData->rssi = eventParam->data[advIndex + PRU_ADV_SERV_DATA_RSSI_OFFSET];
            serviceData->flags = eventParam->data[advIndex + PRU_ADV_SERV_DATA_FLAGS_OFFSET];
            servicePresent = 1u;
            break;
        }
        advIndex += eventParam->data[advIndex] + 1u;
    }while(advIndex < eventParam->dataLen);    
    
    return(servicePresent);
}

/* [] END OF FILE */

