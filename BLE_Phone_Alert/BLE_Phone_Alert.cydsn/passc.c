/*******************************************************************************
* File Name: passc.c
*
* Version 1.0
*
* Description:
*  This file contains Phone Alert Status service related code.
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


uint8 passFlag;
uint16 dscr;
CYBLE_PASS_CHAR_INDEX_T charIndx;
uint8 alertStatus = 0u;
CYBLE_PASS_RS_T ringerSetting = CYBLE_PASS_RS_SILENT;
CYBLE_PASS_CP_T controlPoint = CYBLE_PASS_CP_SILENT;


/* Phone Alert Status service callback */
void PassCallBack(uint32 event, void* eventParam)
{   
    switch(event)
    {
        case CYBLE_EVT_PASSC_NOTIFICATION:
            if(CYBLE_PASS_AS == ((CYBLE_PASS_CHAR_VALUE_T*)eventParam)->charIndex)
            {
                alertStatus = ((CYBLE_PASS_CHAR_VALUE_T*)eventParam)->value->val[0];
                DBG_PRINTF("Alert Status");
            }
            else
            {
                ringerSetting = (CYBLE_PASS_RS_T)((CYBLE_PASS_CHAR_VALUE_T*)eventParam)->value->val[0];
                DBG_PRINTF("Ringer Setting");
            }
            
            if(0u == (alertStatus & (CYBLE_PASS_AS_RINGER | CYBLE_PASS_AS_VIBRATE)))
            {
                Green_LED_Write(ringerSetting);
                Blue_LED_Write(ringerSetting ^ LED_OFF);
            }
            else
            {
                Green_LED_Write(LED_OFF);
                Blue_LED_Write(LED_OFF);   
            }

            DBG_PRINTF(" notification: %2.2x \r\n", ((CYBLE_PASS_CHAR_VALUE_T*)eventParam)->value->val[0]);
            break;
        
        case CYBLE_EVT_PASSC_READ_CHAR_RESPONSE:
            if(CYBLE_PASS_AS == ((CYBLE_PASS_CHAR_VALUE_T*)eventParam)->charIndex)
            {
                alertStatus = ((CYBLE_PASS_CHAR_VALUE_T*)eventParam)->value->val[0];
                passFlag |= FLAG_RD;
                charIndx = CYBLE_PASS_RS;
                DBG_PRINTF("Alert Status");
            }
            else
            {
                ringerSetting = (CYBLE_PASS_RS_T)((CYBLE_PASS_CHAR_VALUE_T*)eventParam)->value->val[0];
                passFlag |= FLAG_DSCR;
                dscr = CYBLE_CCCD_NOTIFICATION;
                charIndx = CYBLE_PASS_AS;
                DBG_PRINTF("Ringer Setting");
            }
            
            if(0u == (alertStatus & (CYBLE_PASS_AS_RINGER | CYBLE_PASS_AS_VIBRATE)))
            {
                Green_LED_Write(ringerSetting);
                Blue_LED_Write(ringerSetting ^ LED_OFF);
            }
            else
            {
                Green_LED_Write(LED_OFF);
                Blue_LED_Write(LED_OFF);   
            }
            
            DBG_PRINTF(" read response: %2.2x \r\n", ((CYBLE_PASS_CHAR_VALUE_T*)eventParam)->value->val[0]);
            break;
            
        case CYBLE_EVT_LNSC_READ_DESCR_RESPONSE:
            if(CYBLE_PASS_AS == ((CYBLE_PASS_CHAR_VALUE_T*)eventParam)->charIndex)
            {
                DBG_PRINTF("Alert Status");
            }
            else
            {
                DBG_PRINTF("Ringer Setting");
            }
            DBG_PRINTF(" read descriptor response: ");
            DBG_PRINTF("%x \r\n", ((CYBLE_PASS_CHAR_VALUE_T*)eventParam)->value->val[0]);
            break;
            
        case CYBLE_EVT_PASSC_WRITE_DESCR_RESPONSE:
            if(CYBLE_PASS_AS == ((CYBLE_PASS_CHAR_VALUE_T*)eventParam)->charIndex)
            {
                passFlag |= FLAG_DSCR;
                dscr = CYBLE_CCCD_NOTIFICATION;
                charIndx = CYBLE_PASS_RS;
                DBG_PRINTF("Alert Status");
            }
            else
            {
                DBG_PRINTF("Ringer Setting");
            }
            DBG_PRINTF(" write descriptor response \r\n");
            break;

		default:
            DBG_PRINTF("unknown PASS event: %lx \r\n", event);
			break;
    }
}


void PassInit(void)
{
    CyBle_PassRegisterAttrCallback(PassCallBack);
    passFlag = 0u;
}


void PassProcess(void)
{   
    if(0u != (passFlag & FLAG_RD))
    {
        passFlag &= (uint8) ~FLAG_RD;
        
        do
        {
            CyBle_ProcessEvents();
        }
        while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);
        
        apiResult = CyBle_PasscGetCharacteristicValue(cyBle_connHandle, charIndx);
        
        if(apiResult != CYBLE_ERROR_OK)
    	{
    		DBG_PRINTF("CyBle_PasscGetCharacteristicValue API Error: ");
            PrintApiResult();
    	}
    	else
    	{
    		if(CYBLE_PASS_AS == charIndx)
            {
                DBG_PRINTF("Alert Status ");
            }
            else
            {
                DBG_PRINTF("Ringer Setting ");
            }
            
            DBG_PRINTF("characteristic Read Request is sent \r\n");
    	}
    }
 
    if(0u != (passFlag & FLAG_DSCR))
    {
        passFlag &= (uint8) ~FLAG_DSCR;
        
        do
        {
            CyBle_ProcessEvents();
        }
        while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);
            
        apiResult = CyBle_PasscSetCharacteristicDescriptor(cyBle_connHandle, charIndx, CYBLE_PASS_CCCD, CYBLE_CCCD_LEN, (uint8*)&dscr);
        
        if(apiResult != CYBLE_ERROR_OK)
    	{
    		DBG_PRINTF("CyBle_PasscSetCharacteristicDescriptor API Error: ");
            PrintApiResult();
    	}
    	else
    	{
    		if(CYBLE_PASS_AS == charIndx)
            {
                DBG_PRINTF("Alert Status notification");
            }
            else
            {
                DBG_PRINTF("Ringer Setting notification");
            }
            
            if(CYBLE_CCCD_NOTIFICATION == dscr)
            {
                DBG_PRINTF(" enable request is sent \r\n");
            }
            else
            {
                DBG_PRINTF(" disable request is sent \r\n");
            }
    	}   
    }
    
    if(0u != (passFlag & FLAG_WR))
    {
        passFlag &= (uint8) ~FLAG_WR;
        
        do
        {
            CyBle_ProcessEvents();
        }
        while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);
        
        apiResult = CyBle_PasscSetCharacteristicValue(cyBle_connHandle, CYBLE_PASS_CP, 1u, (uint8*)&controlPoint);
        
        if(apiResult != CYBLE_ERROR_OK)
    	{
    		DBG_PRINTF("CyBle_PasscSetCharacteristicValue API Error: ");
            PrintApiResult();
    	}
    	else
    	{
            DBG_PRINTF("Control Point write request is sent: ");
            switch(controlPoint)
            {
                case CYBLE_PASS_CP_SILENT:
                    DBG_PRINTF("Silent \r\n");
                    break;
                
                case CYBLE_PASS_CP_MUTE:
                    DBG_PRINTF("Mute Once \r\n");
                    break;
                
                case CYBLE_PASS_CP_CANCEL:
                    DBG_PRINTF("Cancel Silent Mode \r\n");
                    break;
                
                default:
                    break;
            }
    	}
    }
}


/* [] END OF FILE */
