/*******************************************************************************
* File Name: bmss.c
*
* Version 1.0
*
* Description:
*  This file contains the Bond Management Service related code.
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

#include "bmss.h"

CYBLE_GAP_BONDED_DEV_ADDR_LIST_T bdList;
CYBLE_GAP_BD_ADDR_T peerBdAddr;
uint8 unbond = 0u;
uint8 bmcp = 0u;

char8 *authCode = "0abcdef";


/******************************************************************************
##Function Name: BmsInit
*******************************************************************************

Summary:
  Registers the BMS CallBack.

******************************************************************************/
void BmsInit(void)
{
    CyBle_BmsRegisterAttrCallback(BmsCallBack);
}


/******************************************************************************
##Function Name: BmsCallBack
*******************************************************************************

Summary:
  A CallBack function to handle BMS specific events.

******************************************************************************/

void BmsCallBack(uint32 event, void* eventParam)
{   
    switch(event)
    {
        case CYBLE_EVT_BMSS_WRITE_CHAR:
            {
                uint8 features[3u];
                uint8 byteNumber;
                uint8 opCodeFlag;
                uint8 authCodeFlag;
                
                CYBLE_GATT_ERR_CODE_T bmsGattError = CYBLE_GATT_ERR_NONE;
                
                DBG_PRINTF("BMCP write request: ");
                for(i = 0; i < ((CYBLE_BMS_CHAR_VALUE_T*)eventParam)->value->len; i++)
                {
                    DBG_PRINTF("%2.2x ", ((CYBLE_BMS_CHAR_VALUE_T*)eventParam)->value->val[i]);
                }
                DBG_PRINTF("\r\n");
                
                bmcp = ((CYBLE_BMS_CHAR_VALUE_T*)eventParam)->value->val[0];
                
                switch(bmcp)
                {
                    case CYBLE_BMS_BMCP_OPC_RD:
                        DBG_PRINTF("OpCode: Delete bond of requesting device\r\n");
                        byteNumber = CYBLE_BMS_BMFT_FIRST_BYTE;
                        opCodeFlag = (uint8) CYBLE_BMS_BMFT_RD;
                        authCodeFlag = (uint8) CYBLE_BMS_BMFT_RC;
                        break;
                        
                    case CYBLE_BMS_BMCP_OPC_AB:
                        DBG_PRINTF("OpCode: Delete all bonds on server\r\n");
                        byteNumber = CYBLE_BMS_BMFT_SECOND_BYTE;
                        opCodeFlag = (uint8) (CYBLE_BMS_BMFT_AB >> (8 * CYBLE_BMS_BMFT_SECOND_BYTE));
                        authCodeFlag = (uint8) (CYBLE_BMS_BMFT_AC >> (8 * CYBLE_BMS_BMFT_SECOND_BYTE));
                        break;
                            
                    case CYBLE_BMS_BMCP_OPC_BA:
                        DBG_PRINTF("OpCode: Delete all but the active bond on server\r\n");
                        byteNumber = CYBLE_BMS_BMFT_THIRD_BYTE;
                        opCodeFlag = (uint8) (CYBLE_BMS_BMFT_BA >> (8 * CYBLE_BMS_BMFT_THIRD_BYTE));
                        authCodeFlag = (uint8) (CYBLE_BMS_BMFT_BC >> (8 * CYBLE_BMS_BMFT_THIRD_BYTE));
                        break;
                    
                    default:
                        DBG_PRINTF("unsupported BMCP OpCode: 0x%x \r\n", ((CYBLE_CGMS_CHAR_VALUE_T*)eventParam)->value->val[0]);
                        bmsGattError = CYBLE_GATT_ERR_OP_CODE_NOT_SUPPORTED;
            			break;
                }
        
                if(bmsGattError == CYBLE_GATT_ERR_NONE)
                {
                    (void) CyBle_BmssGetCharacteristicValue(CYBLE_BMS_BMFT, CYBLE_BMS_BMFT_SIZE, features);
                    
                    if((opCodeFlag & features[byteNumber]) != 0u)
                    {
                        if((authCodeFlag & features[byteNumber]) != 0u)
                        {   
                            if(((CYBLE_BMS_CHAR_VALUE_T*)eventParam)->value->len == (CYBLE_BMS_AUTH_CODE_SIZE + 1))
                            {
                                for(i = 1u; i < ((CYBLE_BMS_CHAR_VALUE_T*)eventParam)->value->len; i++)
                                {
                                    if(((CYBLE_BMS_CHAR_VALUE_T*)eventParam)->value->val[i] != authCode[i])
                                    {
                                        bmsGattError = CYBLE_GATT_ERR_INSUFFICIENT_AUTHORIZATION;
                                    }
                                }
                            }
                            else
                            {
                                bmsGattError = CYBLE_GATT_ERR_INSUFFICIENT_AUTHORIZATION;
                            }
                        }
                    }
                    else
                    {
                        bmsGattError = CYBLE_GATT_ERR_OP_CODE_NOT_SUPPORTED;
                        DBG_PRINTF("this OpCode not supported\r\n");
                    }
                }
            
                if(bmsGattError == CYBLE_GATT_ERR_NONE)
                {
                    apiResult = CyBle_GapGetPeerBdAddr(cyBle_connHandle.bdHandle, &peerBdAddr);
              		DBG_PRINTF("CyBle_GapGetPeerBdAddr API result: ");
                    PrintApiResult();
                    
                    apiResult = CyBle_GapGetBondedDevicesList(&bdList);
            		DBG_PRINTF("CyBle_GapGetBondedDevicesList API result: ");
                    PrintApiResult();
                }
                else
                {
                    bmcp = 0u;
                }
            
                DBG_PRINTF("Gatt error: ");
                switch(bmsGattError)
                {
                    case CYBLE_GATT_ERR_NONE:
                        DBG_PRINTF("Ok\r\n");
                        break;
                    
                    case CYBLE_GATT_ERR_OP_CODE_NOT_SUPPORTED:
                        DBG_PRINTF("Op Code Not Supported\r\n");
                        break;
                        
                    case CYBLE_GATT_ERR_INSUFFICIENT_AUTHORIZATION:
                        DBG_PRINTF("Insufficient Authorization\r\n");
                        break;
                    
                    default:
                        DBG_PRINTF("unexpected 0x%2.2x\r\n", bmsGattError);
                        break;
                }
            
                ((CYBLE_BMS_CHAR_VALUE_T*)eventParam)->gattErrorCode = bmsGattError;
            }
            break;
            
		default:
            DBG_PRINTF("unknown BMS event: 0x%lx \r\n", event);
			break;
    }
}


/******************************************************************************
##Function Name: BmsProcess
*******************************************************************************

Summary:
  Processes the BM control point requests.

******************************************************************************/
void BmsProcess(void)
{
    if(0u != bmcp)
    {
        switch(bmcp)
        {
            case CYBLE_BMS_BMCP_OPC_RD:
                apiResult = CyBle_GapRemoveDeviceFromWhiteList(&peerBdAddr);
                DBG_PRINTF("CyBle_GapRemoveDeviceFromWhiteList API result: ");
                PrintApiResult();
                break;
                
            case CYBLE_BMS_BMCP_OPC_AB:
                for(i = 0u; i < bdList.count; i++)
                {
                    apiResult = CyBle_GapRemoveOldestDeviceFromBondedList();
                    DBG_PRINTF("CyBle_GapRemoveOldestDeviceFromBondedList API result: ");
                    PrintApiResult();
                }
                break;
                
            case CYBLE_BMS_BMCP_OPC_BA:
                for(i = 0u; i < bdList.count; i++)
                {
                    if((bdList.bdAddrList[i].bdAddr != peerBdAddr.bdAddr) ||
                       (bdList.bdAddrList[i].type != peerBdAddr.type))
                    {
                        apiResult = CyBle_GapRemoveDeviceFromWhiteList(&bdList.bdAddrList[i]);
                        DBG_PRINTF("CyBle_GapRemoveDeviceFromWhiteList API result: ");
                        PrintApiResult();
                    }
                }
                break;
                
            default:
    			break;
        }
        
        bmcp = 0u;
    }
}

/* [] END OF FILE */
