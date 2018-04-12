/*******************************************************************************
* File Name: cgmss.c
*
* Version 1.0
*
* Description:
*  This file contains the Continuous Glucose Monitoring Service related code.
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

#include "cgmss.h"


uint8 cgmsFlag = 0u;
uint8 recCnt = 1u;
uint8 racpOpCode = 0u;
uint8 racpOperator = 0u;
uint8 racpOperand[3u];

CYBLE_CGMS_SOCP_OPC_T socpOpCode;
uint8 socp[13];
uint8 socpLength = 0u;
sfloat socpOpd = 0u;
sfloat* socpOpdPtr = &socpOpd;
uint8 commInterval = 5u;
uint16 socpRecNum = 1u;
CYBLE_CGMS_ALRT_T alrt = {5u,5u,5u,5u,5u,5u};

uint8 attr[24u];
uint8 length = 0u;
CYBLE_CGMS_CGFT_T cgft;
CYBLE_CGMS_SSTM_T sstm;
const CYBLE_TIME_ZONE_T timeZone[CYBLE_TIME_ZONE_VAL_NUM] =
{
    CYBLE_TIME_ZONE_M1200, /* UTC-12:00 */
    CYBLE_TIME_ZONE_M1100, /* UTC-11:00 */
    CYBLE_TIME_ZONE_M1000, /* UTC-10:00 */
    CYBLE_TIME_ZONE_M0930, /* UTC-9:30 */
    CYBLE_TIME_ZONE_M0900, /* UTC-9:00 */
    CYBLE_TIME_ZONE_M0800, /* UTC-8:00 */
    CYBLE_TIME_ZONE_M0700, /* UTC-7:00 */
    CYBLE_TIME_ZONE_M0600, /* UTC-6:00 */
    CYBLE_TIME_ZONE_M0500, /* UTC-5:00 */
    CYBLE_TIME_ZONE_M0430, /* UTC-4:30 */
    CYBLE_TIME_ZONE_M0400, /* UTC-4:00 */
    CYBLE_TIME_ZONE_M0330, /* UTC-3:30 */
    CYBLE_TIME_ZONE_M0300, /* UTC-3:00 */
    CYBLE_TIME_ZONE_M0200, /* UTC-2:00 */
    CYBLE_TIME_ZONE_M0100, /* UTC-1:00 */
    CYBLE_TIME_ZONE_ZERO,  /* UTC+0:00 */
    CYBLE_TIME_ZONE_P0100, /* UTC+1:00 */
    CYBLE_TIME_ZONE_P0200, /* UTC+2:00 */
    CYBLE_TIME_ZONE_P0300, /* UTC+3:00 */
    CYBLE_TIME_ZONE_P0330, /* UTC+3:30 */
    CYBLE_TIME_ZONE_P0400, /* UTC+4:00 */
    CYBLE_TIME_ZONE_P0430, /* UTC+4:30 */
    CYBLE_TIME_ZONE_P0500, /* UTC+5:00 */
    CYBLE_TIME_ZONE_P0530, /* UTC+5:30 */
    CYBLE_TIME_ZONE_P0545, /* UTC+5:45 */
    CYBLE_TIME_ZONE_P0600, /* UTC+6:00 */
    CYBLE_TIME_ZONE_P0630, /* UTC+6:30 */
    CYBLE_TIME_ZONE_P0700, /* UTC+7:00 */
    CYBLE_TIME_ZONE_P0800, /* UTC+8:00 */
    CYBLE_TIME_ZONE_P0845, /* UTC+8:45 */
    CYBLE_TIME_ZONE_P0900, /* UTC+9:00 */
    CYBLE_TIME_ZONE_P0930, /* UTC+9:30 */
    CYBLE_TIME_ZONE_P1000, /* UTC+10:00 */
    CYBLE_TIME_ZONE_P1030, /* UTC+10:30 */
    CYBLE_TIME_ZONE_P1100, /* UTC+11:00 */
    CYBLE_TIME_ZONE_P1130, /* UTC+11:30 */
    CYBLE_TIME_ZONE_P1200, /* UTC+12:00 */
    CYBLE_TIME_ZONE_P1245, /* UTC+12:45 */
    CYBLE_TIME_ZONE_P1300, /* UTC+13:00 */
    CYBLE_TIME_ZONE_P1400  /* UTC+14:00 */
};

uint8 recStatus[REC_NUM] = {REC_STATUS_OK, REC_STATUS_OK, REC_STATUS_OK};

CYBLE_CGMS_CGMT_T cgmt[REC_NUM] =
{
    {   CYBLE_CGMS_GLMT_FLG_TI | 
        CYBLE_CGMS_GLMT_FLG_QA | 
        CYBLE_CGMS_GLMT_FLG_WG | 
        CYBLE_CGMS_GLMT_FLG_CT |
        CYBLE_CGMS_GLMT_FLG_ST, /* flags */
        0xb032u /* 50 mg/dL */, /* CGM Glucose Concentration */
        1u, /* timeOffset */
        CYBLE_CGMS_GLMT_SSA_BL | CYBLE_CGMS_GLMT_SSA_CR | CYBLE_CGMS_GLMT_SSA_RL, /* Sensor Status Annunciation */
        0u, /* CGM Trend Information */
        0u, /* CGM Quality */
    },
    {   CYBLE_CGMS_GLMT_FLG_QA | 
        CYBLE_CGMS_GLMT_FLG_WG | 
        CYBLE_CGMS_GLMT_FLG_CT |
        CYBLE_CGMS_GLMT_FLG_ST, /* flags */
        0xb032u /* 50 mg/dL */, /* CGM Glucose Concentration */
        2u, /* timeOffset */
        CYBLE_CGMS_GLMT_SSA_BL | CYBLE_CGMS_GLMT_SSA_CR | CYBLE_CGMS_GLMT_SSA_RL, /* Sensor Status Annunciation */
        0u, /* CGM Trend Information */
        0u, /* CGM Quality */
    },
    {   CYBLE_CGMS_GLMT_FLG_WG | 
        CYBLE_CGMS_GLMT_FLG_CT |
        CYBLE_CGMS_GLMT_FLG_ST, /* flags */
        0xb032u /* 50 mg/dL */, /* CGM Glucose Concentration */
        3u, /* timeOffset */
        CYBLE_CGMS_GLMT_SSA_BL | CYBLE_CGMS_GLMT_SSA_CR | CYBLE_CGMS_GLMT_SSA_RL, /* Sensor Status Annunciation */
        0u, /* CGM Trend Information */
        0u, /* CGM Quality */
    }
};

void CgmsPrintCharName(CYBLE_CGMS_CHAR_INDEX_T charIndex)
{
    switch(charIndex)
    {
        case CYBLE_CGMS_CGMT:                    
            DBG_PRINTF("CGM Measurement ");
            break;
        
        case CYBLE_CGMS_CGFT:                    
            DBG_PRINTF("CGM Feature ");
            break;
            
        case CYBLE_CGMS_CGST:
            DBG_PRINTF("CGM Status ");
            break;
            
        case CYBLE_CGMS_SSTM:
            DBG_PRINTF("CGM Session Start Time ");
            break;
            
        case CYBLE_CGMS_SRTM:
            DBG_PRINTF("CGM Session Run Time ");
            break;
            
        case CYBLE_CGMS_RACP:
            DBG_PRINTF("RACP ");
            break;
            
        case CYBLE_CGMS_SOCP:
            DBG_PRINTF("SOCP ");
            break;
            
        default:
            DBG_PRINTF("Unknown CGM");
            break;
    }
    
    DBG_PRINTF("characteristic ");
}

/******************************************************************************
##Function Name: CgmsCrc
*******************************************************************************

Summary:
 Calculates a 16-bit CRC value with seed 0xFFFF and polynomial D16+D12+D5+1.

Parameters:
 uint8 length: The length of the data.
 uint8 *dataPtr: The pointer to the data.

Return:
 A uint16 CRC value.

******************************************************************************/
uint16 CgmsCrc(uint8 length, uint8 *dataPtr)
{
    uint8  bit, byte;
    uint16 data, crc = CYBLE_CGMS_CRC_SEED;
    
    for(byte = 0; byte < length; byte++)
    {
        data = (uint16) dataPtr[byte];
        for(bit = 0u; bit < 8u; bit++)
        {
            if(0u != ((crc ^ data) & 0x0001u))
            {
                crc = (crc >> 1u) ^ CYBLE_CGMS_CRC_POLY;
            }
            else
            {
                crc >>= 1u;
            }
            
            data >>= 1u;
        }
    }

    return(crc);
}


/******************************************************************************
##Function Name: CgmsCrcLength
*******************************************************************************

Summary:
  Adds the CRC value to the end of the attribute payload 
  and returns the actual length of the attribute
  taking into account whether the CRC calculation feature is active.

  Uses the above declared CgmsCrc();

Parameters:
 uint8 length: The length of the attribute payload without CRC.
 uint8 *dataPtr: The pointer to the attribute payload.

Return:
 A uint8 length of the attribute, within CRC if the CRC feature is active.

******************************************************************************/
uint8 CgmsCrcLength(uint8 length, uint8* dataPtr)
{
    if(0u != (cgft.feature & CYBLE_CGMS_CGFT_FTR_EC))
    {
        CyBle_Set16ByPtr(&dataPtr[length], CgmsCrc(length, dataPtr));
        length += 2;
    }
    
    return(length);
}


/******************************************************************************
##Function Name: CgmsCrcCheck
*******************************************************************************

Summary:
  Checks the CRC value in the received attribute payload
  taking into account whether the CRC calculation feature is active.

  Uses the above declared CgmsCrc();

Parameters:
  uint16 attrSize: The length of the attribute payload without CRC.
  CYBLE_GATT_VALUE_T *gattValue: The pointer to the GATT value.

Return:
  CYBLE_GATT_ERR_CODE_T result of the CRC checking if the CRC feature is active.
  Following are the possible error codes:
   CYBLE_GATT_ERR_NONE               CRC is correct.
   CYBLE_GATT_ERR_INVALID_CRC        CRC doesn't match.
   CYBLE_GATT_ERR_MISSING_CRC        CRC is absent.

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CgmsCrcCheck(uint16 attrSize, CYBLE_GATT_VALUE_T *gattValue)
{
    CYBLE_GATT_ERR_CODE_T gattError = CYBLE_GATT_ERR_NONE;
    
    if(0u != (CYBLE_CGMS_CGFT_FTR_EC & cgft.feature))
    {
        DBG_PRINTF("Check CRC: ");
        
        if((attrSize + CYBLE_CGMS_CRC_SIZE) == gattValue->len)
        {   
            if(CyBle_Get16ByPtr(&gattValue->val[attrSize]) != 
                CgmsCrc(attrSize, gattValue->val))
            {
                gattError = CYBLE_GATT_ERR_INVALID_CRC;
                DBG_PRINTF("invalid CRC\r\n");
            }
            else
            {
                DBG_PRINTF("ok\r\n");
            }
        }
        else
        {
            gattError = CYBLE_GATT_ERR_MISSING_CRC;
            DBG_PRINTF("missing CRC\r\n");
        }
    }
    
    return(gattError);
}


/******************************************************************************
##Function Name: CgmsInit
*******************************************************************************

Summary:
  Does the initialization of the CGM Service.
  Registers CGMS CallBack and reads the initial CGM feature characteristic.

******************************************************************************/
void CgmsInit(void)
{
    uint8 acgft[6u];
    
    CyBle_CgmsRegisterAttrCallback(CgmsCallBack);
    
    apiResult = CyBle_CgmssGetCharacteristicValue(CYBLE_CGMS_CGFT, 6u, acgft);
    if(apiResult != CYBLE_ERROR_OK)
	{
		DBG_PRINTF("CyBle_CgmssGetCharacteristicValue API Error: ");
        PrintApiResult();
	}
    else
    {
        cgft.feature = (uint32) acgft[0];
        cgft.feature |= (uint32) (acgft[1] << 8u);
        cgft.feature |= (uint32) (acgft[2] << 16u);
        cgft.type = acgft[3] & CYBLE_CGMS_CGFT_TYPE_MASK;
        cgft.sampLoc = (acgft[3] & CYBLE_CGMS_CGFT_SL_MASK) >> CYBLE_CGMS_CGFT_SL_SHIFT;
    }
}


/******************************************************************************
##Function Name: CgmsCallBack
*******************************************************************************

Summary:
  A CallBack function to handle CGMS specific events.

******************************************************************************/
void CgmsCallBack(uint32 event, void* eventParam)
{    
    switch(event)
    {
        case CYBLE_EVT_CGMSS_WRITE_CHAR:
            {
                uint16 attrSize = 0u;
                CYBLE_GATT_ERR_CODE_T cgmsGattError = CYBLE_GATT_ERR_NONE;
                
                CgmsPrintCharName(((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->charIndex);
                DBG_PRINTF("is written: ");
                for(i = 0; i < ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->len; i++)
                {
                    DBG_PRINTF("%2.2x ", ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[i]);
                }
                DBG_PRINTF("\r\n");
            
                switch(((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->charIndex)
                {
                    case CYBLE_CGMS_SSTM:
                        cgmsGattError = CgmsCrcCheck(CYBLE_CGMS_SSTM_SIZE, ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value);
                        
                        sstm.sst.year = CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[0u]);
                        sstm.sst.month = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[2u];
                        sstm.sst.day = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[3u];
                        sstm.sst.hours = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[4u];
                        sstm.sst.minutes = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[5u];
                        sstm.sst.seconds = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[6u];
                        sstm.timeZone = (CYBLE_TIME_ZONE_T)((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[7u];
                        sstm.dstOffset = (CYBLE_DSTOFFSET_T)((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[8u];
                        
                        cgmsGattError = CYBLE_GATT_ERR_INVALID_PDU;
                        for(i = 0; i < CYBLE_TIME_ZONE_VAL_NUM; i++)
                        {
                            if(timeZone[i] == sstm.timeZone)
                            {
                                cgmsGattError = CYBLE_GATT_ERR_NONE;
                            }
                        }
                        
                        if(cgmsGattError != CYBLE_GATT_ERR_NONE)
                        {
                            DBG_PRINTF("invalid PDU\r\n");   
                        }
                        break;
                    
                    case CYBLE_CGMS_RACP:                    
                        cgmsFlag |= CGMS_FLAG_RACP;
                        
                        DBG_PRINTF("Opcode: ");
                        racpOpCode = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[0];
                        switch(racpOpCode)
                        {
                            case CYBLE_CGMS_RACP_OPC_REPORT_REC:
                                DBG_PRINTF("Report stored records \r\n");
                                break;
                                
                            case CYBLE_CGMS_RACP_OPC_DELETE_REC:
                                DBG_PRINTF("Delete stored records \r\n");
                                break;
                                
                            case CYBLE_CGMS_RACP_OPC_ABORT_OPN:
                                DBG_PRINTF("Abort operation \r\n");
                                break;
                                
                            case CYBLE_CGMS_RACP_OPC_REPORT_NUM_REC:
                                DBG_PRINTF("Report number of stored records \r\n");
                                break;
                                
                            default:
                                DBG_PRINTF("Unknown \r\n");
                                break;
                        }
                        
                        DBG_PRINTF("Operator: ");
                        racpOperator = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1];
                        attr[3u] = CYBLE_CGMS_RACP_RSP_SUCCESS;
                        switch(racpOperator)
                        {
                            case CYBLE_CGMS_RACP_OPR_NULL:
                                DBG_PRINTF("Null \r\n");
                                break;
                            
                            case CYBLE_CGMS_RACP_OPR_LAST:
                                DBG_PRINTF("Last record \r\n");
                                break;
                                
                            case CYBLE_CGMS_RACP_OPR_FIRST:
                                DBG_PRINTF("First record \r\n");
                                break;
                                
                            case CYBLE_CGMS_RACP_OPR_ALL:
                                DBG_PRINTF("All records \r\n");
                                if(((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->len > 2u)
                                {
                                    attr[3u] = CYBLE_CGMS_RACP_RSP_INV_OPD;
                                }
                                break;
                            
                            case CYBLE_CGMS_RACP_OPR_LESS:
                                DBG_PRINTF("Less than or equal to \r\n");
                                DBG_PRINTF("Operand: ");
                                if(((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->len == 5u)
                                {
                                    racpOperand[0] = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[2];
                                    if(CYBLE_CGMS_RACP_OPD_1 == racpOperand[0u])
                                    {
                                        racpOperand[1] = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[3];
                                        DBG_PRINTF("Time Offset: %x \r\n", racpOperand[1u]);
                                    }
                                    else
                                    {
                                        DBG_PRINTF("Unknown \r\n");
                                    }
                                }
                                else
                                {
                                    attr[3u] = CYBLE_CGMS_RACP_RSP_INV_OPD;
                                    DBG_PRINTF("Invalid \r\n");
                                }
                                break;
                                
                            case CYBLE_CGMS_RACP_OPR_GREAT:
                                DBG_PRINTF("Greater than or equal to \r\n");
                                DBG_PRINTF("Operand: ");
                                if(((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->len == 5u)
                                {
                                    racpOperand[0u] = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[2u];
                                    if(CYBLE_CGMS_RACP_OPD_1 == racpOperand[0u])
                                    {
                                        racpOperand[1u] = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[3u];
                                        DBG_PRINTF("Time Offset: %x \r\n", racpOperand[1u]);
                                    }
                                    else
                                    {
                                        DBG_PRINTF("Unknown \r\n");
                                    }
                                }
                                else
                                {
                                    attr[3u] = CYBLE_CGMS_RACP_RSP_INV_OPD;
                                    DBG_PRINTF("Invalid \r\n");
                                }
                                break;
                            
                            case CYBLE_CGMS_RACP_OPR_WITHIN:
                                DBG_PRINTF("Within range of (inclusive) \r\n");
                                DBG_PRINTF("Operand: ");
                                if(((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->len == 7u)
                                {
                                    racpOperand[0] = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[2u];
                                    if(CYBLE_CGMS_RACP_OPD_1 == racpOperand[0u])
                                    {
                                        racpOperand[1] = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[3u];
                                        racpOperand[2] = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[5u];
                                        DBG_PRINTF("Time Offsets: %x, %x \r\n", racpOperand[1u], racpOperand[2u]);
                                    }
                                    else
                                    {
                                        DBG_PRINTF("Unknown \r\n");
                                    }
                                }
                                else
                                {
                                    attr[3u] = CYBLE_CGMS_RACP_RSP_INV_OPD;
                                    DBG_PRINTF("Invalid \r\n");
                                }
                                break;
                        
                            default:
                                DBG_PRINTF("Unknown \r\n");
                                break;
                        }
                        break;
                        
                    case CYBLE_CGMS_SOCP:
                        
                        socpOpCode = (CYBLE_CGMS_SOCP_OPC_T)((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[0];
                        socp[2u] = CYBLE_CGMS_SOCP_RSP_SUCCESS;
                        
                        switch(socpOpCode)
                        {
                            case CYBLE_CGMS_SOCP_OPC_STSN:
                            case CYBLE_CGMS_SOCP_OPC_SPSN:
                                attrSize = CYBLE_CGMS_SOCP_OPC_SN;
                                break;
                            
                            case CYBLE_CGMS_SOCP_OPC_SINT:
                                attrSize = CYBLE_CGMS_SOCP_OPC_IN;
                                break;
                            
                            case CYBLE_CGMS_SOCP_OPC_SHAL:
                            case CYBLE_CGMS_SOCP_OPC_SLAL:
                            case CYBLE_CGMS_SOCP_OPC_SHPO:
                            case CYBLE_CGMS_SOCP_OPC_SHPR:
                            case CYBLE_CGMS_SOCP_OPC_SDEC:
                            case CYBLE_CGMS_SOCP_OPC_SINC:
                                attrSize = CYBLE_CGMS_SOCP_OPC_AL;
                                break;
                            
                            case CYBLE_CGMS_SOCP_OPC_SGCV:
                                attrSize = CYBLE_CGMS_SOCP_OPC_CV;
                                break;
                            
                            default:
                                socp[2u] = CYBLE_CGMS_SOCP_RSP_UNSPRT_OPC;
                                break;
                        }
                        
                        if(socp[2u] == CYBLE_CGMS_SOCP_RSP_SUCCESS)
                        {
                            cgmsGattError = CgmsCrcCheck(attrSize, ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value);
                        }
                                                
                        if(cgmsGattError == CYBLE_GATT_ERR_NONE)
                        {
                            cgmsFlag |= CGMS_FLAG_SOCP;
                            DBG_PRINTF("Opcode: ");
                            
                            switch(socpOpCode)
                            {
                                case CYBLE_CGMS_SOCP_OPC_GINT:
                                    DBG_PRINTF("Get CGM Communication Interval\r\n");
                                    DBG_PRINTF("Response: Communication Interval: 0x%2.2x\r\n", commInterval);
                                    socp[0u] = CYBLE_CGMS_SOCP_OPC_RINT;
                                    socp[1u] = commInterval;
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_SINT:
                                    DBG_PRINTF("Set CGM Communication Interval\r\n");
                                    commInterval = ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1];
                                    DBG_PRINTF("Operand: 0x%2.2x \r\n", commInterval);
                                    socp[1u] = CYBLE_CGMS_SOCP_OPC_SINT;
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_SGCV:
                                    DBG_PRINTF("Set Glucose Calibration Value\r\n"); 
                                    socp[1u] = CYBLE_CGMS_SOCP_OPC_SGCV;
                                    DBG_PRINTF("Operand:\r\n Glucose Concentration 0x%4.4x\r\n",
                                        CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1]));
                                    DBG_PRINTF(" Calibration Time 0x%4.4x\r\n",
                                        CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[3]));
                                    DBG_PRINTF(" Sample Location 0x%2.2x\r\n", ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[5]);
                                    DBG_PRINTF(" Next Calibration 0x%4.4x\r\n",
                                        CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[6]));
                                    DBG_PRINTF(" Calibration Data Record Number 0x%4.4x\r\n",
                                        CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[8]));
                                    DBG_PRINTF(" Calibration Status 0x%2.2x\r\n", ((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[10]);
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_GGCV:
                                    DBG_PRINTF("Get Glucose Calibration Value\r\n");
                                    DBG_PRINTF("Operand: Calibration Data Record Number: 0x%4.4x \r\n",
                                            CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1]));
                                    if(0xfffeu == CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1]))
                                    {
                                        socp[0u] = CYBLE_CGMS_SOCP_OPC_RSPC;
                                        socp[1u] = CYBLE_CGMS_SOCP_OPC_GGCV;
                                        socp[2u] = CYBLE_CGMS_SOCP_RSP_POOR;
                                        socpLength = 3u;
                                    }
                                    else
                                    {
                                    
                                        if(0xffffu == CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1]))
                                        {
                                            socpRecNum++;
                                        }
                                        else
                                        {
                                            socpRecNum = CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1]);
                                        }
                                        
                                        socp[0u] = CYBLE_CGMS_SOCP_OPC_RGCV;
                                        DBG_PRINTF("Response:\r\n Glucose Concentration 0x004e(78mg/dL)\r\n");
                                        CyBle_Set16ByPtr(&socp[1u], 0x004eu);
                                        DBG_PRINTF(" Calibration Time 0x0005 (5 minutes)\r\n");
                                        CyBle_Set16ByPtr(&socp[3u], 0x0005u);
                                        DBG_PRINTF(" Sample Location 0x06\r\n");
                                        socp[5] = 0x06u;
                                        DBG_PRINTF(" Next Calibration 0x0005 (5 minutes)\r\n");
                                        CyBle_Set16ByPtr(&socp[6u], 0x0005u);
                                        DBG_PRINTF(" Calibration Data Record Number 0x%4.4x \r\n", socpRecNum);
                                        CyBle_Set16ByPtr(&socp[8u], socpRecNum);
                                        DBG_PRINTF(" Calibration Status 0x00\r\n");
                                        socp[10u] = 0x00u;
                                        socpLength = 11u;
                                    }
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_GHAL:
                                    DBG_PRINTF("Get Patient High Alert Level\r\n"); 
                                    DBG_PRINTF("Response: Patient High Alert Level: 0x%4.4x\r\n", alrt.hal);
                                    socp[0u] = CYBLE_CGMS_SOCP_OPC_RHAL;
                                    CyBle_Set16ByPtr(&socp[1u], alrt.hal);
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_SHAL:
                                    DBG_PRINTF("Set Patient High Alert Level\r\n");
                                    socpOpd = CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1]);
                                    socpOpdPtr = &alrt.hal;
                                    DBG_PRINTF("Operand: Patient High Alert Level: 0x%4.4x \r\n", socpOpd);
                                    socp[1u] = CYBLE_CGMS_SOCP_OPC_SHAL;
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_GLAL:
                                    DBG_PRINTF("Get Patient Low Alert Level\r\n"); 
                                    DBG_PRINTF("Response: Patient High Alert Level: 0x%4.4x\r\n", alrt.lal);
                                    socp[0u] = CYBLE_CGMS_SOCP_OPC_RLAL;
                                    CyBle_Set16ByPtr(&socp[1u], alrt.lal);
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_SLAL:
                                    DBG_PRINTF("Set Patient Low Alert Level\r\n");
                                    socpOpd = CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1]);
                                    socpOpdPtr = &alrt.lal;
                                    DBG_PRINTF("Operand: Patient Low Alert Level: 0x%4.4x \r\n", socpOpd);
                                    socp[1u] = CYBLE_CGMS_SOCP_OPC_SLAL;
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_GHPO:
                                    DBG_PRINTF("Get Hypo Alert Level\r\n"); 
                                    DBG_PRINTF("Response: Hypo Alert Level: 0x%4.4x\r\n", alrt.hpo);
                                    socp[0u] = CYBLE_CGMS_SOCP_OPC_RHPO;
                                    CyBle_Set16ByPtr(&socp[1u], alrt.hpo);
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_SHPO:
                                    DBG_PRINTF("Set Hypo Alert Level\r\n");
                                    socpOpd = CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1]);
                                    socpOpdPtr = &alrt.hpo;
                                    DBG_PRINTF("Operand: Hypo Alert Level: 0x%4.4x \r\n", socpOpd);
                                    socp[1u] = CYBLE_CGMS_SOCP_OPC_SHPO;
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_GHPR:
                                    DBG_PRINTF("Get Hyper Alert Level\r\n"); 
                                    DBG_PRINTF("Response: Hyper Alert Level: 0x%4.4x\r\n", alrt.hpr);
                                    socp[0u] = CYBLE_CGMS_SOCP_OPC_RHPR;
                                    CyBle_Set16ByPtr(&socp[1u], alrt.hpr);
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_SHPR:
                                    DBG_PRINTF("Set Hyper Alert Level\r\n");
                                    socpOpd = CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1]);
                                    socpOpdPtr = &alrt.hpr;
                                    DBG_PRINTF("Operand: Hyper Alert Level: 0x%4.4x \r\n", socpOpd);
                                    socp[1u] = CYBLE_CGMS_SOCP_OPC_SHPR;
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_GDEC:
                                    DBG_PRINTF("Get Rate of Decrease Alert Level\r\n"); 
                                    DBG_PRINTF("Response: Rate of Decrease Alert Level: 0x%4.4x\r\n", alrt.dec);
                                    socp[0u] = CYBLE_CGMS_SOCP_OPC_RDEC;
                                    CyBle_Set16ByPtr(&socp[1u], alrt.dec);
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_SDEC:
                                    DBG_PRINTF("Set Rate of Decrease Alert Level\r\n");
                                    socpOpd = CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1]);
                                    socpOpdPtr = &alrt.dec;
                                    DBG_PRINTF("Operand: Rate of Decrease Alert Level: 0x%4.4x \r\n", socpOpd);
                                    socp[1u] = CYBLE_CGMS_SOCP_OPC_SDEC;
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_GINC:
                                    DBG_PRINTF("Get Rate of Increase Alert Level\r\n"); 
                                    DBG_PRINTF("Response: Rate of Increase Alert Level: 0x%4.4x\r\n", alrt.inc);
                                    socp[0u] = CYBLE_CGMS_SOCP_OPC_RINC;
                                    CyBle_Set16ByPtr(&socp[1u], alrt.inc);
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_SINC:
                                    DBG_PRINTF("Set Rate of Increase Alert Level\r\n");
                                    socpOpd = CyBle_Get16ByPtr(&((CYBLE_CGMS_CHAR_VALUE_T *)eventParam)->value->val[1]);
                                    socpOpdPtr = &alrt.inc;
                                    DBG_PRINTF("Operand: Rate of Increase Alert Level: 0x%4.4x \r\n", socpOpd);
                                    socp[1u] = CYBLE_CGMS_SOCP_OPC_SINC;
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_RDSA:
                                    DBG_PRINTF("Reset Device Specific Alert\r\n"); 
                                    socp[1u] = CYBLE_CGMS_SOCP_OPC_RDSA;
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_STSN:
                                    DBG_PRINTF("Start the Session\r\n");
                                    apiResult = CyBle_CgmssGetCharacteristicValue(CYBLE_CGMS_CGST, 7u, attr);
                                    if(apiResult != CYBLE_ERROR_OK)
                                    {
                                        DBG_PRINTF("CyBle_CgmssGetCharacteristicValue API Error: ");
                                        PrintApiResult();
                                    }
                                    
                                    if(0u != (attr[2] & (uint8) CYBLE_CGMS_GLMT_SSA_SS))
                                    {
                                        attr[2u] &= (uint8) ~CYBLE_CGMS_GLMT_SSA_SS;
                                    
                                        apiResult = CyBle_CgmssSetCharacteristicValue(CYBLE_CGMS_CGST, CgmsCrcLength(5u, attr), attr);
                                        if(apiResult != CYBLE_ERROR_OK)
                                        {
                                            DBG_PRINTF("CyBle_CgmssGetCharacteristicValue API Error: ");
                                            PrintApiResult();
                                        }
                                    }
                                    else
                                    {
                                        socp[2u] = CYBLE_CGMS_SOCP_RSP_NO_COMPL;
                                    }
                                    socp[1u] = CYBLE_CGMS_SOCP_OPC_STSN;
                                    break;
                                    
                                case CYBLE_CGMS_SOCP_OPC_SPSN:
                                    DBG_PRINTF("Stop the Session\r\n");
                                    apiResult = CyBle_CgmssGetCharacteristicValue(CYBLE_CGMS_CGST, 7u, attr);
                                    if(apiResult != CYBLE_ERROR_OK)
                                    {
                                        DBG_PRINTF("CyBle_CgmssGetCharacteristicValue API Error: ");
                                        PrintApiResult();
                                    }
                                    
                                    attr[2u] |= (uint8) CYBLE_CGMS_GLMT_SSA_SS;
                                    
                                    apiResult = CyBle_CgmssSetCharacteristicValue(CYBLE_CGMS_CGST, CgmsCrcLength(5u, attr), attr);
                                    if(apiResult != CYBLE_ERROR_OK)
                                    {
                                        DBG_PRINTF("CyBle_CgmssGetCharacteristicValue API Error: ");
                                        PrintApiResult();
                                    }
                                    
                                    socp[1u] = CYBLE_CGMS_SOCP_OPC_SPSN;
                                    break;
                                    
                                default:
                                    DBG_PRINTF("Not Supported \r\n");
                                    break;
                            }
                        }
                        break;
                        
                    default:
                        break;
                }
                
                ((CYBLE_CGMS_CHAR_VALUE_T*)eventParam)->gattErrorCode = cgmsGattError;
            }
            break;
        
        case CYBLE_EVT_CGMSS_NOTIFICATION_ENABLED:
            DBG_PRINTF("Glucose Notification is Enabled \r\n");
            break;
                
        case CYBLE_EVT_CGMSS_NOTIFICATION_DISABLED:
            DBG_PRINTF("Glucose Notification is Disabled \r\n");
            break;
            
        case CYBLE_EVT_CGMSS_INDICATION_ENABLED:
            DBG_PRINTF("RACP Indication is Enabled \r\n");
            break;
                
        case CYBLE_EVT_CGMSS_INDICATION_DISABLED:
            DBG_PRINTF("RACP Indication is Disabled \r\n");
            break;
            
        case CYBLE_EVT_CGMSS_INDICATION_CONFIRMED:
            DBG_PRINTF("RACP Indication is Confirmed \r\n");
            break;

		default:
            DBG_PRINTF("unknown CGMS event: %lx \r\n", event);
			break;
    }
}


/******************************************************************************
##Function Name: CgmsSendCgmtNtf
*******************************************************************************

Summary:
  Packs the payload and sends a notification
  of the CGM Measurement characteristic.

  Uses the above declared CgmsCrc();

Parameters:
  CYBLE_CGMS_CGMT_T cgmt - The CGM Measurement characteristic value structure.

Return:
  None. 

******************************************************************************/
void CgmsSendCgmtNtf(CYBLE_CGMS_CGMT_T cgmt)
{
    uint8 pdu[sizeof(CYBLE_CGMS_CGMT_T)];
    uint8 ptr, b;
    
    /* "Flags" octet goes second */
	pdu[1u] = cgmt.flags;
    
    /* CGM Glucose Concentration is third and forth bytes */
	CyBle_Set16ByPtr(&pdu[2u], cgmt.gluConc);
    
    /* Time Offset consumes next two bytes */
    CyBle_Set16ByPtr(&pdu[4u], cgmt.timeOffset);
    
    ptr = 6u; /* size of size + flags + gluConc + timeOffset) */
        
    /* if "Warning-Octet present" flag is set */
    if(0u != (cgmt.flags & CYBLE_CGMS_GLMT_FLG_WG))
    {
        /* set the next byte of CGM Measurement characteristic value */
        pdu[ptr] = cgmt.ssa & CYBLE_CGMS_GLMT_SSA_WGM;
        ptr++;
    }
    
    if(0u != (cgmt.flags & CYBLE_CGMS_GLMT_FLG_CT))
    {
        pdu[ptr] = (cgmt.ssa & CYBLE_CGMS_GLMT_SSA_CTM) >> CYBLE_CGMS_GLMT_SSA_CTS;
        ptr++;
    }
    
    if(0u != (cgmt.flags & CYBLE_CGMS_GLMT_FLG_ST))
    {
        pdu[ptr] = (cgmt.ssa & CYBLE_CGMS_GLMT_SSA_STM) >> CYBLE_CGMS_GLMT_SSA_STS;
        ptr++;
    }
    
    if((0u != (cgft.feature & CYBLE_CGMS_CGFT_FTR_TI)) && 
            (0u != (cgmt.flags & CYBLE_CGMS_GLMT_FLG_TI)))
    {
        CyBle_Set16ByPtr(&pdu[ptr], cgmt.trend);
        ptr += 2;
    }
    
    if((0u != (cgft.feature & CYBLE_CGMS_CGFT_FTR_QA)) && 
            (0u != (cgmt.flags & CYBLE_CGMS_GLMT_FLG_QA)))
    {
        CyBle_Set16ByPtr(&pdu[ptr], cgmt.quality);
        ptr += 2;
    }
    
    if(0u != (cgft.feature & CYBLE_CGMS_CGFT_FTR_EC))
    {
        /* Size includes crc field */
	    pdu[0u] = ptr + 2;
        CyBle_Set16ByPtr(&pdu[ptr], CgmsCrc(ptr, pdu));
        ptr = pdu[0u];
    }
    else
    {
        pdu[0u] = ptr;
    }
        
    do
    {
        CyBle_ProcessEvents();
    }
    while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);
    
    apiResult = CyBle_CgmssSendNotification(cyBle_connHandle, CYBLE_CGMS_CGMT, ptr, pdu);
	if(apiResult != CYBLE_ERROR_OK)
	{
		DBG_PRINTF("CgmsSendCgmtNtf API Error: ");
        PrintApiResult();
	}
    else
    {
        DBG_PRINTF("Cgmt Ntf: ");
        for(b = 0; b < ptr; b++)
        {
            DBG_PRINTF("%2.2x ", pdu[b]);
        }
        DBG_PRINTF("\r\n");
    }
}


/******************************************************************************
##Function Name: CgmsRacpOpCodeProcess
*******************************************************************************

Summary:
  Processes the CGM record depending on RACP OpCode.

Parameters:
  uint8 recNum: the number of the CGM record.

Return:
  None. 

******************************************************************************/
void CgmsRacpOpCodeProcess(uint8 recNum)
{
    attr[3u] = CYBLE_CGMS_RACP_RSP_SUCCESS;
    
    switch(racpOpCode)
    {
        case CYBLE_CGMS_RACP_OPC_REPORT_REC:
            CgmsSendCgmtNtf(cgmt[recNum]);
            break;
            
        case CYBLE_CGMS_RACP_OPC_REPORT_NUM_REC:
            recCnt++;
            break;
            
        case CYBLE_CGMS_RACP_OPC_DELETE_REC:
            recStatus[recNum] = REC_STATUS_DELETED;
            break;
            
        default:
            attr[3u] = CYBLE_CGMS_RACP_RSP_UNSPRT_OPC;
            break;
    }
}


/******************************************************************************
##Function Name: CgmsProcess
*******************************************************************************

Summary:
  Processes the CGM control points requests.

******************************************************************************/
void CgmsProcess(void)
{
    uint8 recNum;
            
    if((cgmsFlag & CGMS_FLAG_RACP) != 0u)
    {
        switch(racpOperator)
        {
            case CYBLE_CGMS_RACP_OPR_NULL:
                switch(racpOpCode)
                {
                    case CYBLE_CGMS_RACP_OPC_REPORT_REC:
                    case CYBLE_CGMS_RACP_OPC_DELETE_REC:
                    case CYBLE_CGMS_RACP_OPC_REPORT_NUM_REC:
                        attr[3u] = CYBLE_CGMS_RACP_RSP_INV_OPR;
                        break;
                        
                    default:
                        break;
                }
                break;
            
            case CYBLE_CGMS_RACP_OPR_LAST:
                if(CYBLE_CGMS_RACP_RSP_SUCCESS == attr[3u])
                {
                    CgmsRacpOpCodeProcess(REC_NUM - 1u);
                }
                break;
                
            case CYBLE_CGMS_RACP_OPR_FIRST:
                if(CYBLE_CGMS_RACP_RSP_SUCCESS == attr[3u])
                {
                    CgmsRacpOpCodeProcess(0u);
                }
                break;
                
            case CYBLE_CGMS_RACP_OPR_ALL:
                if(CYBLE_CGMS_RACP_RSP_SUCCESS == attr[3u])
                {
                    attr[3u] = CYBLE_CGMS_RACP_RSP_NO_REC;
                    recCnt = 0u;
                    for(recNum = 0u; recNum < REC_NUM; recNum++)
                    {  
                        if(REC_STATUS_OK == recStatus[recNum])
                        {
                            CgmsRacpOpCodeProcess(recNum);
                        } 
                    }
                }
                break;
                
            case CYBLE_CGMS_RACP_OPR_LESS:
                if(CYBLE_CGMS_RACP_RSP_SUCCESS == attr[3u])
                {
                    attr[3] = CYBLE_CGMS_RACP_RSP_NO_REC;
                    
                    if(CYBLE_CGMS_RACP_OPD_1 == racpOperand[0u])
                    {
                        recCnt = 0u;
                        for(recNum = 0; recNum < REC_NUM; recNum++)
                        {
                            if((cgmt[recNum].timeOffset <= racpOperand[1u]) && (REC_STATUS_OK == recStatus[recNum]))
                            {
                                CgmsRacpOpCodeProcess(recNum);
                            }    
                        }
                    }
                    else
                    {
                        attr[3u] = CYBLE_CGMS_RACP_RSP_UNSPRT_OPD;
                    }
                }
                break;
                
            case CYBLE_CGMS_RACP_OPR_GREAT:
                if(CYBLE_CGMS_RACP_RSP_SUCCESS == attr[3u])
                {
                    attr[3u] = CYBLE_CGMS_RACP_RSP_NO_REC;
                    
                    if(CYBLE_CGMS_RACP_OPD_1 == racpOperand[0u])
                    {
                        recCnt = 0u;
                        for(recNum = 0; recNum < REC_NUM; recNum++)
                        {
                            if((cgmt[recNum].timeOffset >= racpOperand[1u]) && (REC_STATUS_OK == recStatus[recNum]))
                            {
                                CgmsRacpOpCodeProcess(recNum);
                            }    
                        }
                    }
                    else
                    {
                        attr[3u] = CYBLE_CGMS_RACP_RSP_UNSPRT_OPD;
                    }
                }
                break;
                
            case CYBLE_CGMS_RACP_OPR_WITHIN:
                if(CYBLE_CGMS_RACP_RSP_SUCCESS == attr[3u])
                {
                    attr[3u] = CYBLE_CGMS_RACP_RSP_NO_REC;
                    
                    if(CYBLE_CGMS_RACP_OPD_1 == racpOperand[0u])
                    {
                        if(racpOperand[1] > racpOperand[2u])
                        {
                            attr[3u] = CYBLE_CGMS_RACP_RSP_INV_OPD;   
                        }
                        else
                        {
                            for(recNum = 0; recNum < REC_NUM; recNum++)
                            {
                                if((cgmt[recNum].timeOffset >= racpOperand[1u]) &&
                                    (cgmt[recNum].timeOffset <= racpOperand[2u]) &&
                                        (REC_STATUS_OK == recStatus[recNum]))
                                {
                                    CgmsRacpOpCodeProcess(recNum);    
                                }
                            }
                        }
                    }
                    else
                    {
                        attr[3u] = CYBLE_CGMS_RACP_RSP_UNSPRT_OPD;
                    }
                }
                break;
                
            default:
                attr[3u] = CYBLE_CGMS_RACP_RSP_UNSPRT_OPR;
                break;
        }
        
        attr[1u] = CYBLE_CGMS_RACP_OPR_NULL;
        length = 4u;
        
        if(CYBLE_CGMS_RACP_OPC_REPORT_NUM_REC == racpOpCode)
        {
            attr[0u] = CYBLE_CGMS_RACP_OPC_NUM_REC_RSP;
            attr[2u] = recCnt;
            attr[3u] = 0u;
        }
        else
        {
            attr[0u] = CYBLE_CGMS_RACP_OPC_RSP_CODE;
            attr[2u] = racpOpCode;
        }
        
        do
        {
            CyBle_ProcessEvents();
        }
        while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);
        
        apiResult = CyBle_CgmssSendIndication(cyBle_connHandle, CYBLE_CGMS_RACP, length, attr);
        if(apiResult != CYBLE_ERROR_OK)
    	{
    		DBG_PRINTF("CyBle_CgmssSendIndication API Error: ");
            PrintApiResult();
    	}
        else
        {
            DBG_PRINTF("RACP Ind: ");
            for(i = 0; i < length; i++)
            {
                DBG_PRINTF("%2.2x ", attr[i]);
            }
            DBG_PRINTF("\r\n");
        }
        
        cgmsFlag &= (uint8) ~CGMS_FLAG_RACP;
    }
    
    if(0u != (cgmsFlag & CGMS_FLAG_SOCP))
    {
        switch(socpOpCode)
        {
            case CYBLE_CGMS_SOCP_OPC_GINT:
                socpLength = 2u;
                break;
            
            case CYBLE_CGMS_SOCP_OPC_RDSA:
            case CYBLE_CGMS_SOCP_OPC_SPSN:
                socpOpd = 0u;    
                socpOpdPtr = &socpOpd;
            case CYBLE_CGMS_SOCP_OPC_SINT:
            case CYBLE_CGMS_SOCP_OPC_SGCV:
            case CYBLE_CGMS_SOCP_OPC_SHAL:
            case CYBLE_CGMS_SOCP_OPC_SLAL:
            case CYBLE_CGMS_SOCP_OPC_SHPO:
            case CYBLE_CGMS_SOCP_OPC_SHPR:
            case CYBLE_CGMS_SOCP_OPC_SDEC:
            case CYBLE_CGMS_SOCP_OPC_SINC:
                
                switch(socpOpd)
                {
                    case SFLOAT_NAN:
                    case SFLOAT_NRES:
                    case SFLOAT_PINF:
                    case SFLOAT_NINF:
                    case SFLOAT_RSRV:
                        socp[2u] = CYBLE_CGMS_SOCP_RSP_POOR;
                        break;
                        
                    default:
                        *socpOpdPtr = socpOpd;
                        break;
                }
            case CYBLE_CGMS_SOCP_OPC_STSN:
                socp[0u] = CYBLE_CGMS_SOCP_OPC_RSPC;
            case CYBLE_CGMS_SOCP_OPC_GHAL:
            case CYBLE_CGMS_SOCP_OPC_GLAL:
            case CYBLE_CGMS_SOCP_OPC_GHPO:
            case CYBLE_CGMS_SOCP_OPC_GHPR:
            case CYBLE_CGMS_SOCP_OPC_GDEC:
            case CYBLE_CGMS_SOCP_OPC_GINC:
                socpLength = 3u;
                break;
            
            default:
                break;
        }
        
        do
        {
            CyBle_ProcessEvents();
        }
        while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);
        
        socpLength = CgmsCrcLength(socpLength, socp);
        apiResult = CyBle_CgmssSendIndication(cyBle_connHandle, CYBLE_CGMS_SOCP, socpLength, socp);
        if(apiResult != CYBLE_ERROR_OK)
    	{
    		DBG_PRINTF("CyBle_CgmssSendIndication API Error: ");
            PrintApiResult();
    	}
        else
        {
            DBG_PRINTF("SOCP Ind: ");
            for(i = 0; i < socpLength; i++)
            {
                DBG_PRINTF("%2.2x ", socp[i]);
            }
            DBG_PRINTF("\r\n");
        }
        
        cgmsFlag &= (uint8) ~CGMS_FLAG_SOCP;
    }
}


/* [] END OF FILE */
