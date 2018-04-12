/*******************************************************************************
* File Name: glss.c
*
* Version 1.0
*
* Description:
*  This file contains GLS service related code.
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


/* Global variables */
uint8 racpCommand = 0u;
uint8 racpOpCode = 0u;
uint8 racpOperator = 0u;
uint8 racpFilterType;
uint16 seqNum1;
uint16 seqNum2;
CYBLE_DATE_TIME_T userFacingTime1;
CYBLE_DATE_TIME_T userFacingTime2;
uint8 racpInd[4u];
uint8 recCnt = 1u;


/* Record Status array */
uint8 recStat[CYBLE_GLS_REC_NUM] =
{
    CYBLE_GLS_REC_STAT_OK,
    CYBLE_GLS_REC_STAT_OK,
    CYBLE_GLS_REC_STAT_OK,
    CYBLE_GLS_REC_STAT_OK,
    CYBLE_GLS_REC_STAT_OK,
    CYBLE_GLS_REC_STAT_OK,
    CYBLE_GLS_REC_STAT_OK,
    CYBLE_GLS_REC_STAT_OK,
    CYBLE_GLS_REC_STAT_OK,
    CYBLE_GLS_REC_STAT_OK,
    CYBLE_GLS_REC_STAT_OK
};


/* Glucose Measurement records */
CYBLE_GLS_GLMT_T glsGlucose[CYBLE_GLS_REC_NUM] =
{
    {CYBLE_GLS_GLMT_FLG_TOP | CYBLE_GLS_GLMT_FLG_SSA,
        0u, {2014u, 7u, 27, 20u, 30u, 40u}, 0, 0xb032u /* 50 mg/dL */,
        (CYBLE_GLS_GLMT_TYPE_CWB | (CYBLE_GLS_GLMT_SL_FR << CYBLE_GLS_GLMT_SL_SHIFT)), CYBLE_GLS_GLMT_SSA_BTL},
    {CYBLE_GLS_GLMT_FLG_TOP | CYBLE_GLS_GLMT_FLG_GLC | CYBLE_GLS_GLMT_FLG_GCU | CYBLE_GLS_GLMT_FLG_CIF,
        1u, {2014u, 7u, 27, 20u, 30u, 40u}, 1, 0xd032u /* 50 mmol/L (50*10^-3 mol/L)*/,
        (CYBLE_GLS_GLMT_TYPE_CWB | (CYBLE_GLS_GLMT_SL_FR << CYBLE_GLS_GLMT_SL_SHIFT)), 0u},
    {CYBLE_GLS_GLMT_FLG_TOP | CYBLE_GLS_GLMT_FLG_GLC | CYBLE_GLS_GLMT_FLG_SSA,
        2u, {2014u, 7u, 27, 20u, 30u, 40u}, 2, 0xb032u /* 50 mg/dL (50*10^-5 kg/L)*/,
        (CYBLE_GLS_GLMT_TYPE_CWB | (CYBLE_GLS_GLMT_SL_FR << CYBLE_GLS_GLMT_SL_SHIFT)), CYBLE_GLS_GLMT_SSA_BTL},
    {CYBLE_GLS_GLMT_FLG_TOP | CYBLE_GLS_GLMT_FLG_SSA,
        3u, {2014u, 7u, 27, 20u, 30u, 40u}, 60, 0xb032u /* 50 mg/dL */,
        (CYBLE_GLS_GLMT_TYPE_CWB | (CYBLE_GLS_GLMT_SL_FR << CYBLE_GLS_GLMT_SL_SHIFT)), CYBLE_GLS_GLMT_SSA_BTL},
    {CYBLE_GLS_GLMT_FLG_TOP | CYBLE_GLS_GLMT_FLG_GLC | CYBLE_GLS_GLMT_FLG_GCU | CYBLE_GLS_GLMT_FLG_CIF,
        4u, {2014u, 7u, 27, 20u, 30u, 40u}, 60, 0xd032u /* 50 mmol/L (50*10^-3 mol/L)*/,
        (CYBLE_GLS_GLMT_TYPE_CWB | (CYBLE_GLS_GLMT_SL_FR << CYBLE_GLS_GLMT_SL_SHIFT)), 0u},
    {CYBLE_GLS_GLMT_FLG_TOP | CYBLE_GLS_GLMT_FLG_GLC | CYBLE_GLS_GLMT_FLG_SSA,
        5u, {2014u, 7u, 27, 20u, 30u, 40u}, 59, 0xb032u /* 50 mg/dL (50*10^-5 kg/L)*/,
        (CYBLE_GLS_GLMT_TYPE_CWB | (CYBLE_GLS_GLMT_SL_FR << CYBLE_GLS_GLMT_SL_SHIFT)), CYBLE_GLS_GLMT_SSA_BTL},
    {CYBLE_GLS_GLMT_FLG_TOP | CYBLE_GLS_GLMT_FLG_SSA,
        6u, {2014u, 7u, 27, 20u, 30u, 40u}, -60, 0xb032u /* 50 mg/dL */,
        (CYBLE_GLS_GLMT_TYPE_CWB | (CYBLE_GLS_GLMT_SL_FR << CYBLE_GLS_GLMT_SL_SHIFT)), CYBLE_GLS_GLMT_SSA_BTL},
    {CYBLE_GLS_GLMT_FLG_TOP | CYBLE_GLS_GLMT_FLG_GLC | CYBLE_GLS_GLMT_FLG_GCU | CYBLE_GLS_GLMT_FLG_CIF,
        7u, {2014u, 7u, 27, 20u, 30u, 40u}, -60, 0xd032u /* 50 mmol/L (50*10^-3 mol/L)*/,
        (CYBLE_GLS_GLMT_TYPE_CWB | (CYBLE_GLS_GLMT_SL_FR << CYBLE_GLS_GLMT_SL_SHIFT)), 0u},
    {CYBLE_GLS_GLMT_FLG_TOP | CYBLE_GLS_GLMT_FLG_GLC | CYBLE_GLS_GLMT_FLG_SSA,
        8u, {2014u, 7u, 27, 20u, 30u, 40u}, -58, 0xb032u /* 50 mg/dL (50*10^-5 kg/L)*/,
        (CYBLE_GLS_GLMT_TYPE_CWB | (CYBLE_GLS_GLMT_SL_FR << CYBLE_GLS_GLMT_SL_SHIFT)), CYBLE_GLS_GLMT_SSA_BTL},
    {CYBLE_GLS_GLMT_FLG_TOP | CYBLE_GLS_GLMT_FLG_GLC | CYBLE_GLS_GLMT_FLG_SSA,
        9u, {2014u, 7u, 27, 20u, 32u, 45u}, 10u, 0xb037u /* 55 mg/dL (50*10^-5 kg/L)*/,
        (CYBLE_GLS_GLMT_TYPE_CWB | (CYBLE_GLS_GLMT_SL_FR << CYBLE_GLS_GLMT_SL_SHIFT)), CYBLE_GLS_GLMT_SSA_BTL},
    {CYBLE_GLS_GLMT_FLG_TOP | CYBLE_GLS_GLMT_FLG_GLC | CYBLE_GLS_GLMT_FLG_SSA | CYBLE_GLS_GLMT_FLG_CIF,
        10u, {2014u, 7u, 27, 20u, 33u, 46u}, 11u, 0xb032u /* 50 mg/dL (50*10^-5 kg/L)*/,
        (CYBLE_GLS_GLMT_TYPE_CWB | (CYBLE_GLS_GLMT_SL_FR << CYBLE_GLS_GLMT_SL_SHIFT)), CYBLE_GLS_GLMT_SSA_BTL}
};


/* Glucose Measurement Context records */
CYBLE_GLS_GLMC_T glsGluCont[CYBLE_GLS_REC_NUM] =
{
    {CYBLE_GLS_GLMC_FLG_EXT, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u},
    {CYBLE_GLS_GLMC_FLG_CBID | CYBLE_GLS_GLMC_FLG_MEAL | CYBLE_GLS_GLMC_FLG_TNH |
        CYBLE_GLS_GLMC_FLG_EXR | CYBLE_GLS_GLMC_FLG_MED | CYBLE_GLS_GLMC_FLG_A1C | CYBLE_GLS_GLMC_FLG_EXT,
        1u, 0u, CYBLE_GLS_GLMC_CBID_DRINK, 0xd032u /* 50 gram (50*10^-3 kg)*/,
        CYBLE_GLS_GLMC_MEAL_FAST, CYBLE_GLS_GLMC_TESTER_LAB | (CYBLE_GLS_GLMC_HEALTH_US << CYBLE_GLS_GLMC_HEALTH_SHIFT),
        780u /* 13 min */, 78u /* 78% */,
        CYBLE_GLS_GLMC_MEDID_IAI, 0xa032u /* 50 mgram (50*10^-6 kg)*/,
        0x0032u /* 50% */},
    {CYBLE_GLS_GLMC_FLG_EXT, 2u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u},
    {CYBLE_GLS_GLMC_FLG_EXT, 3u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u},
    {CYBLE_GLS_GLMC_FLG_CBID | CYBLE_GLS_GLMC_FLG_MEAL | CYBLE_GLS_GLMC_FLG_TNH |
        CYBLE_GLS_GLMC_FLG_EXR | CYBLE_GLS_GLMC_FLG_MED | CYBLE_GLS_GLMC_FLG_A1C | CYBLE_GLS_GLMC_FLG_EXT,
        4u, 0u, CYBLE_GLS_GLMC_CBID_DRINK, 0xd032u /* 50 gram (50*10^-3 kg)*/,
        CYBLE_GLS_GLMC_MEAL_FAST, CYBLE_GLS_GLMC_TESTER_LAB | (CYBLE_GLS_GLMC_HEALTH_US << CYBLE_GLS_GLMC_HEALTH_SHIFT),
        780u /* 13 min */, 78u /* 78% */,
        CYBLE_GLS_GLMC_MEDID_IAI, 0xa032u /* 50 mgram (50*10^-6 kg)*/,
        0x0032u /* 50% */},
    {CYBLE_GLS_GLMC_FLG_EXT, 5u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u},
    {CYBLE_GLS_GLMC_FLG_EXT, 6u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u},
    {CYBLE_GLS_GLMC_FLG_CBID | CYBLE_GLS_GLMC_FLG_MEAL | CYBLE_GLS_GLMC_FLG_TNH |
        CYBLE_GLS_GLMC_FLG_EXR | CYBLE_GLS_GLMC_FLG_MED | CYBLE_GLS_GLMC_FLG_A1C | CYBLE_GLS_GLMC_FLG_EXT,
        7u, 0u, CYBLE_GLS_GLMC_CBID_DRINK, 0xd032u /* 50 gram (50*10^-3 kg)*/,
        CYBLE_GLS_GLMC_MEAL_FAST, CYBLE_GLS_GLMC_TESTER_LAB | (CYBLE_GLS_GLMC_HEALTH_US << CYBLE_GLS_GLMC_HEALTH_SHIFT),
        780u /* 13 min */, 78u /* 78% */,
        CYBLE_GLS_GLMC_MEDID_IAI, 0xa032u /* 50 mgram (50*10^-6 kg)*/,
        0x0032u /* 50% */},
    {CYBLE_GLS_GLMC_FLG_EXT, 8u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u},
    {CYBLE_GLS_GLMC_FLG_EXT, 9u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u},
    {CYBLE_GLS_GLMC_FLG_CBID | CYBLE_GLS_GLMC_FLG_MEAL | CYBLE_GLS_GLMC_FLG_TNH |
        CYBLE_GLS_GLMC_FLG_EXR | CYBLE_GLS_GLMC_FLG_MED | CYBLE_GLS_GLMC_FLG_A1C | CYBLE_GLS_GLMC_FLG_EXT,
        10u, 0u, CYBLE_GLS_GLMC_CBID_DRINK, 0xd032u /* 50 gram (50*10^-3 kg)*/,
        CYBLE_GLS_GLMC_MEAL_FAST, CYBLE_GLS_GLMC_TESTER_LAB | (CYBLE_GLS_GLMC_HEALTH_US << CYBLE_GLS_GLMC_HEALTH_SHIFT),
        780u /* 13 min */, 78u /* 78% */,
        CYBLE_GLS_GLMC_MEDID_IAI, 0xa032u /* 50 mgram (50*10^-6 kg)*/,
        0x0032u /* 50% */}
};


/*******************************************************************************
* Function Name: Operand
********************************************************************************
*
* Summary:
*   Prints the Filter Type and Filter Parameters of the RACP Operand.
*
* Parameters:
*   uint8* val - RACP value.
*
* Return:
*   None
*
*******************************************************************************/
void Operand(uint8* val)
{
    DBG_PRINTF("Filter type: \r\n");
    racpFilterType = val[2u];
    if(CYBLE_GLS_RACP_OPD_1 == racpFilterType)
    {
        DBG_PRINTF("Sequence number: ");
        seqNum1 = CyBle_Get16ByPtr(&val[3u]);
        DBG_PRINTF("%x", seqNum1);
        if(CYBLE_GLS_RACP_OPR_WITHIN == racpOperator)
        {
            seqNum2 = CyBle_Get16ByPtr(&val[5u]);
            DBG_PRINTF(", %x", seqNum2);
        }
        DBG_PRINTF("\r\n");
    }
    else if(CYBLE_GLS_RACP_OPD_1 == racpFilterType)
    {
        DBG_PRINTF("User Facing Time \r\n");
        userFacingTime1.year = CyBle_Get16ByPtr(&val[3u]);
        userFacingTime1.month = val[5u];
        userFacingTime1.day = val[6u];
        userFacingTime1.hours = val[7u];
        userFacingTime1.minutes = val[8u];
        userFacingTime1.seconds = val[9u];

        DBG_PRINTF(" %d a.d. %d/%d %d:%d:%d \r\n", userFacingTime1.year,
                                               userFacingTime1.month,
                                               userFacingTime1.day,
                                               userFacingTime1.hours,
                                               userFacingTime1.minutes,
                                               userFacingTime1.seconds);

        if(CYBLE_GLS_RACP_OPR_WITHIN == racpOperator)
        {
            userFacingTime2.year = CyBle_Get16ByPtr(&val[10u]);
            userFacingTime2.month = val[12u];
            userFacingTime2.day = val[13u];
            userFacingTime2.hours = val[14u];
            userFacingTime2.minutes = val[15u];
            userFacingTime2.seconds = val[16u];

            DBG_PRINTF(" %d a.d. %d/%d %d:%d:%d \r\n", userFacingTime2.year,
                                                   userFacingTime2.month,
                                                   userFacingTime2.day,
                                                   userFacingTime2.hours,
                                                   userFacingTime2.minutes,
                                                   userFacingTime2.seconds);
        }
    }
    else
    {
        DBG_PRINTF("Unknown \r\n");
    }
}


/******************************************************************************
##Function Name: GlsInit
*******************************************************************************

Summary:
  Registers the GlS CallBack.

******************************************************************************/
void GlsInit(void)
{
    CyBle_GlsRegisterAttrCallback(GlsCallBack);
}


/*******************************************************************************
* Function Name: GlsCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service specific events from
*   Glucose Service.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
* Return:
*  None.
*
********************************************************************************/
void GlsCallBack(uint32 event, void* eventParam)
{
    uint8 i;
    switch(event)
    {
        case CYBLE_EVT_GLSS_WRITE_CHAR:
            DBG_PRINTF("RACP is written: ");
            for(i = 0; i < ((CYBLE_GLS_CHAR_VALUE_T *)eventParam)->value->len; i++)
            {
                DBG_PRINTF("%2.2x ", ((CYBLE_GLS_CHAR_VALUE_T *)eventParam)->value->val[i]);
            }
            DBG_PRINTF("\r\n");

            if((0u == racpCommand) || (CYBLE_GLS_RACP_OPC_ABORT_OPN == ((CYBLE_GLS_CHAR_VALUE_T *)eventParam)->value->val[0]))
            {
                racpCommand = 1u;
                DBG_PRINTF("Opcode: ");
                racpOpCode = ((CYBLE_GLS_CHAR_VALUE_T *)eventParam)->value->val[0];
                switch(racpOpCode)
                {
                    case CYBLE_GLS_RACP_OPC_REPORT_REC:
                        DBG_PRINTF("Report stored records \r\n");
                        break;

                    case CYBLE_GLS_RACP_OPC_DELETE_REC:
                        DBG_PRINTF("Delete stored records \r\n");
                        break;

                    case CYBLE_GLS_RACP_OPC_ABORT_OPN:
                        DBG_PRINTF("Abort operation \r\n");
                        break;

                    case CYBLE_GLS_RACP_OPC_REPORT_NUM_REC:
                        DBG_PRINTF("Report number of stored records \r\n");
                        break;

                    default:
                        DBG_PRINTF("Unknown \r\n");
                        break;
                }

                DBG_PRINTF("Operator: ");
                racpOperator = ((CYBLE_GLS_CHAR_VALUE_T *)eventParam)->value->val[1];

                switch(racpOperator)
                {
                    case CYBLE_GLS_RACP_OPR_NULL:
                        DBG_PRINTF("Null \r\n");
                        break;

                    case CYBLE_GLS_RACP_OPR_LAST:
                        DBG_PRINTF("Last record \r\n");
                        if(((CYBLE_GLS_CHAR_VALUE_T *)eventParam)->value->len > 2)
                        {
                            racpCommand = 2u; /* invalid operand */
                        }
                        break;

                    case CYBLE_GLS_RACP_OPR_FIRST:
                        DBG_PRINTF("First record \r\n");
                        if(((CYBLE_GLS_CHAR_VALUE_T *)eventParam)->value->len > 2)
                        {
                            racpCommand = 2u; /* invalid operand */
                        }
                        break;

                    case CYBLE_GLS_RACP_OPR_ALL:
                        DBG_PRINTF("All records \r\n");
                        if(((CYBLE_GLS_CHAR_VALUE_T *)eventParam)->value->len > 2)
                        {
                            racpCommand = 2u; /* invalid operand */
                        }
                        break;

                    case CYBLE_GLS_RACP_OPR_LESS:
                        DBG_PRINTF("Less than or equal to \r\n");
                        Operand(((CYBLE_GLS_CHAR_VALUE_T *)eventParam)->value->val);
                        break;


                    case CYBLE_GLS_RACP_OPR_GREAT:
                        DBG_PRINTF("Greater than or equal to \r\n");
                        Operand(((CYBLE_GLS_CHAR_VALUE_T *)eventParam)->value->val);
                        break;

                    case CYBLE_GLS_RACP_OPR_WITHIN:
                        DBG_PRINTF("Within range of (inclusive) \r\n");
                        Operand(((CYBLE_GLS_CHAR_VALUE_T *)eventParam)->value->val);
                        break;

                    default:
                        DBG_PRINTF("Unknown \r\n");
                        break;
                }
            }
            else
            {
                DBG_PRINTF("new request is not accepted because of current request is still being processed \r\n");
            }

            break;

        case CYBLE_EVT_GLSS_NOTIFICATION_ENABLED:
            if(CYBLE_GLS_GLMT == ((CYBLE_GLS_CHAR_VALUE_T*)eventParam)->charIndex)
            {
                DBG_PRINTF("Glucose Measurement");
            }
            else if(CYBLE_GLS_GLMC == ((CYBLE_GLS_CHAR_VALUE_T*)eventParam)->charIndex)
            {
                DBG_PRINTF("Glucose Measurement Context");
            }
            else
            {
                DBG_PRINTF("Other GLS");
            }
            DBG_PRINTF(" characteristic notification is enabled \r\n");
            break;

        case CYBLE_EVT_GLSS_NOTIFICATION_DISABLED:
            if(CYBLE_GLS_GLMT == ((CYBLE_GLS_CHAR_VALUE_T*)eventParam)->charIndex)
            {
                DBG_PRINTF("Glucose Measurement");
            }
            else if(CYBLE_GLS_GLMC == ((CYBLE_GLS_CHAR_VALUE_T*)eventParam)->charIndex)
            {
                DBG_PRINTF("Glucose Measurement Context");
            }
            else
            {
                DBG_PRINTF("Other GLS");
            }
            DBG_PRINTF(" characteristic notification is disabled \r\n");
            break;

        case CYBLE_EVT_GLSS_INDICATION_ENABLED:
            if(CYBLE_GLS_RACP == ((CYBLE_GLS_CHAR_VALUE_T*)eventParam)->charIndex)
            {
                DBG_PRINTF("RACP");
            }
            else
            {
                DBG_PRINTF("Other GLS");
            }
            DBG_PRINTF(" characteristic indication is enabled \r\n");
            break;

        case CYBLE_EVT_GLSS_INDICATION_DISABLED:
            if(CYBLE_GLS_RACP == ((CYBLE_GLS_CHAR_VALUE_T*)eventParam)->charIndex)
            {
                DBG_PRINTF("RACP");
            }
            else
            {
                DBG_PRINTF("Other GLS");
            }
            DBG_PRINTF(" characteristic indication is disabled \r\n");
            break;

        case CYBLE_EVT_GLSS_INDICATION_CONFIRMED:
            if(CYBLE_GLS_RACP == ((CYBLE_GLS_CHAR_VALUE_T*)eventParam)->charIndex)
            {
                DBG_PRINTF("RACP");
            }
            else
            {
                DBG_PRINTF("Other GLS");
            }
            DBG_PRINTF(" characteristic indication is confirmed \r\n");
            break;

        default:
            DBG_PRINTF("unknown GLS event: %lx \r\n", event);
            break;
    }
}


/*******************************************************************************
* Function Name: GlsNtf
********************************************************************************
*
* Summary:
*   Sends the Glucose Measurement notification and the Glucose Measurement
*   Context notification (if the appropriate bit is set).
*
* Parameters:
*   uint8 num - number of record to notify.
*
* Return:
*   None
*
*******************************************************************************/
void GlsNtf(uint8 num)
{
    uint8 pdu[sizeof(CYBLE_GLS_GLMT_T)]; /* GLMC size is also 17 bytes */
    uint8 ptr;

    /* flags field is always the first byte */
    pdu[0u] = glsGlucose[num].flags;

    /* Sequence number is always the second and third bytes */
    CyBle_Set16ByPtr(&pdu[1u], glsGlucose[num].seqNum);

    /* Base Time consumes the next seven bytes */
    (void)memcpy(&pdu[3u], &glsGlucose[num].baseTime, sizeof(glsGlucose[num].baseTime));

    ptr = 10u; /* 3 + 7 (size of baseTime) */

    /* if the Time Offset Present flag is set */
    if(0u != (glsGlucose[num].flags & CYBLE_GLS_GLMT_FLG_TOP))
    {
        /* and set the full 2-bytes Time Offset value */
        (void)memcpy(&pdu[ptr], &glsGlucose[num].timeOffset, sizeof(glsGlucose[num].timeOffset));
        /* the next data will be located beginning from 3rd byte */
        ptr += sizeof(glsGlucose[num].timeOffset);
    }


    if(0u != (glsGlucose[num].flags & CYBLE_GLS_GLMT_FLG_GLC))
    {
        (void)memcpy(&pdu[ptr], &glsGlucose[num].gluConc, sizeof(glsGlucose[num].gluConc));
        ptr += sizeof(glsGlucose[num].gluConc);
        pdu[ptr] = glsGlucose[num].tnsl;
        ptr += 1u; /* uint8 tnsl */
    }

    if(0u != (glsGlucose[num].flags & CYBLE_GLS_GLMT_FLG_SSA))
    {
        (void)memcpy(&pdu[ptr], &glsGlucose[num].ssa, sizeof(glsGlucose[num].ssa));
        ptr += 2u; /* uint16 ssa */
    }
    
    do
    {
        CyBle_ProcessEvents();
    }
    while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);

    if(CYBLE_ERROR_OK != (apiResult = CyBle_GlssSendNotification(cyBle_connHandle, CYBLE_GLS_GLMT, ptr, pdu)))
    {
        DBG_PRINTF("CyBle_GlssSendNotification API Error: ");
        PrintApiResult();
    }
    else
    {
        DBG_PRINTF("Glucose Ntf: %d \r\n", glsGlucose[num].seqNum);
        racpInd[3] = CYBLE_GLS_RACP_RSP_SUCCESS;
    }

    if(0u != (glsGlucose[num].flags & CYBLE_GLS_GLMT_FLG_CIF))
    {
        /* flags field is always the first byte */
        pdu[0u] = glsGluCont[num].flags;

        /* Sequence number is always the second and third bytes */
        CyBle_Set16ByPtr(&pdu[1u], glsGluCont[num].seqNum);

        /* if the Time Offset Present flag is set */
        if(0u != (glsGluCont[num].flags & CYBLE_GLS_GLMC_FLG_EXT))
        {
            /* and set the 1-byte Extended Flags */
            pdu[3u] = glsGluCont[num].exFlags;
            /* the next data will be located beginning from 3rd byte */
            ptr = 4u;
        }
        else
        {
            ptr = 3u; /* otherwise the next data will be located beginning from 2nd byte */
        }

        if(0u != (glsGluCont[num].flags & CYBLE_GLS_GLMC_FLG_CBID))
        {
            pdu[ptr] = glsGluCont[num].cbId;
            ptr += 1u; /* uint8 cbId */
            (void)memcpy(&pdu[ptr], &glsGluCont[num].cbhdr, sizeof(glsGluCont[num].cbhdr));
            ptr += sizeof(glsGluCont[num].cbhdr);
        }

        if(0u != (glsGluCont[num].flags & CYBLE_GLS_GLMC_FLG_MEAL))
        {
            pdu[ptr] = glsGluCont[num].meal;
            ptr += 1u; /* uint8 meal */
        }

        if(0u != (glsGluCont[num].flags & CYBLE_GLS_GLMC_FLG_TNH))
        {
            pdu[ptr] = glsGluCont[num].tnh;
            ptr += 1u; /* uint8 tnh */
        }

        if(0u != (glsGluCont[num].flags & CYBLE_GLS_GLMC_FLG_EXR))
        {
            CyBle_Set16ByPtr(&pdu[ptr], glsGluCont[num].exDur);
            ptr += 2u; /* uint16 exDur */
            pdu[ptr] = glsGluCont[num].exInt;
            ptr += 1u; /* uint8 exInt */
        }

        if(0u != (glsGluCont[num].flags & CYBLE_GLS_GLMC_FLG_MED))
        {
            pdu[ptr] = glsGluCont[num].medId;
            ptr += 1u; /* uint8 medId */
            (void)memcpy(&pdu[ptr], &glsGluCont[num].medic, sizeof(glsGluCont[num].medic));
            ptr += sizeof(glsGluCont[num].medic);
        }

        if(0u != (glsGluCont[num].flags & CYBLE_GLS_GLMC_FLG_A1C))
        {
            (void)memcpy(&pdu[ptr], &glsGluCont[num].hba1c, sizeof(glsGluCont[num].hba1c));
            ptr += sizeof(glsGluCont[num].hba1c);
        }
        
        do
        {
            CyBle_ProcessEvents();
        }
        while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);
        
        if(CYBLE_ERROR_OK != (apiResult = CyBle_GlssSendNotification(cyBle_connHandle, CYBLE_GLS_GLMC, ptr, pdu)))
        {
            DBG_PRINTF("CyBle_GlssSendNotification API Error: ");
        PrintApiResult();
        }
        else
        {
            DBG_PRINTF("Glucose Context Ntf: %d \r\n", glsGlucose[num].seqNum);
        }
    }
}


/*******************************************************************************
* Function Name: GlsInd
********************************************************************************
*
* Summary:
*   Sends the GLS RACP indication with the status of the current request.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void GlsInd(void)
{
    racpInd[1] = CYBLE_GLS_RACP_OPR_NULL;

    if(CYBLE_GLS_RACP_OPC_REPORT_NUM_REC == racpOpCode)
    {
        racpInd[0] = CYBLE_GLS_RACP_OPC_NUM_REC_RSP;
        racpInd[2] = recCnt;
        racpInd[3] = 0;
    }
    else
    {
        racpInd[0] = CYBLE_GLS_RACP_OPC_RSP_CODE;
        racpInd[2] = racpOpCode;
    }
    
    do
    {
        CyBle_ProcessEvents();
    }
    while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);

    if(CYBLE_ERROR_OK != (apiResult = CyBle_GlssSendIndication(cyBle_connHandle, CYBLE_GLS_RACP, 4, racpInd)))
    {
        DBG_PRINTF("CyBle_GlssSendIndication API Error: ");
        PrintApiResult();
    }
    else
    {
        DBG_PRINTF("RACP Ind: %d %d %d %d \r\n", racpInd[0], racpInd[1], racpInd[2], racpInd[3]);
    }
}


/*******************************************************************************
* Function Name: TimeComparison
********************************************************************************
*
* Summary:
*   Compares the user facing time with time specified in
*   Glucose measurement record.
*
* Parameters:
*   CYBLE_DATE_TIME_T glmtTime - Time specified in Glucose measurement record.
*   CYBLE_DATE_TIME_T userFacingTime - User facing time.
*
* Return:
*   CYBLE_DATE_TIME_COMP_T retVal - result of time comparison.
*
*******************************************************************************/
CYBLE_DATE_TIME_COMP_T TimeComparison(CYBLE_DATE_TIME_T glmtTime, CYBLE_DATE_TIME_T userFacingTime)
{
    CYBLE_DATE_TIME_COMP_T retVal = CYBLE_TIME_LESS;

    if(glmtTime.year == userFacingTime.year)
    {
        if(glmtTime.month == userFacingTime.month)
        {
            if(glmtTime.day == userFacingTime.day)
            {
                if(glmtTime.hours == userFacingTime.hours)
                {
                    if(glmtTime.minutes == userFacingTime.minutes)
                    {
                        if(glmtTime.seconds == userFacingTime.seconds)
                        {
                            retVal = CYBLE_TIME_EQUAL;
                        }
                        else if(glmtTime.seconds > userFacingTime.seconds)
                        {
                            retVal = CYBLE_TIME_GREAT;
                        }
                        else
                        {
                            /* CYBLE_TIME_LESS */
                        }
                    }
                    else if(glmtTime.minutes > userFacingTime.minutes)
                    {
                        retVal = CYBLE_TIME_GREAT;
                    }
                    else
                    {
                        /* CYBLE_TIME_LESS */
                    }
                }
                else if(glmtTime.hours > userFacingTime.hours)
                {
                    retVal = CYBLE_TIME_GREAT;
                }
                else
                {
                    /* CYBLE_TIME_LESS */
                }
            }
            else if(glmtTime.day > userFacingTime.day)
            {
                retVal = CYBLE_TIME_GREAT;
            }
            else
            {
                /* CYBLE_TIME_LESS */
            }
        }
        else if(glmtTime.month > userFacingTime.month)
        {
            retVal = CYBLE_TIME_GREAT;
        }
        else
        {
            /* CYBLE_TIME_LESS */
        }
    }
    else if(glmtTime.year > userFacingTime.year)
    {
        retVal = CYBLE_TIME_GREAT;
    }
    else
    {
        /* CYBLE_TIME_LESS */
    }

    return (retVal);
}

/*******************************************************************************
* Function Name: OpCodeOperation
********************************************************************************
*
* Summary:
*   Doing operation in accordance to opcode and status or current record.
*
* Parameters:
*   uint8 i - index of record.
*
* Return:
*   None
*
*******************************************************************************/
void OpCodeOperation(uint8 i)
{
    if(CYBLE_GLS_REC_STAT_OK == recStat[i])
    {
        switch(racpOpCode)
        {
            case CYBLE_GLS_RACP_OPC_REPORT_REC:
                GlsNtf(i);
                break;

            case CYBLE_GLS_RACP_OPC_DELETE_REC:
                recStat[i] = CYBLE_GLS_REC_STAT_DELETED;
                racpInd[3] = CYBLE_GLS_RACP_RSP_SUCCESS;
                break;

            case CYBLE_GLS_RACP_OPC_REPORT_NUM_REC:
                recCnt++;
                break;

            default:
                racpInd[3] = CYBLE_GLS_RACP_RSP_UNSPRT_OPC;
                break;
        }
    }
}

/*******************************************************************************
* Function Name: GlsProcess
********************************************************************************
*
* Summary:
*   Processes the GLS RACP request.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void GlsProcess(void)
{
    if(0u != racpCommand)
    {
        uint8 i;
        CyDelay(7);
        CyBle_ProcessEvents();

        recCnt = 0u;
        racpInd[3] = CYBLE_GLS_RACP_RSP_NO_REC;

        switch(racpOperator)
        {
            case CYBLE_GLS_RACP_OPR_NULL:
                switch(racpOpCode)
                {
                    case CYBLE_GLS_RACP_OPC_ABORT_OPN:
                        racpInd[3] = CYBLE_GLS_RACP_RSP_SUCCESS;
                        break;

                    default:
                        racpInd[3] = CYBLE_GLS_RACP_RSP_INV_OPR;
                        break;
                }

                GlsInd();
                break;

            case CYBLE_GLS_RACP_OPR_LAST:
                if(2u == racpCommand)
                {
                    racpInd[3] = CYBLE_GLS_RACP_RSP_INV_OPD;
                }
                else
                {
                    OpCodeOperation(CYBLE_GLS_REC_NUM - 1);
                }

                GlsInd();
                break;

            case CYBLE_GLS_RACP_OPR_FIRST:
                if(2u == racpCommand)
                {
                    racpInd[3] = CYBLE_GLS_RACP_RSP_INV_OPD;
                }
                else
                {
                    OpCodeOperation(0);
                }

                GlsInd();
                break;

            case CYBLE_GLS_RACP_OPR_ALL:
                if(2u == racpCommand)
                {
                    racpInd[3] = CYBLE_GLS_RACP_RSP_INV_OPD;
                }
                else
                {
                    for(i = 0; i < CYBLE_GLS_REC_NUM; i++)
                    {
                        OpCodeOperation(i);
                    }
                }

                GlsInd();
                break;

            case CYBLE_GLS_RACP_OPR_LESS:
                if(CYBLE_GLS_RACP_OPD_1 == racpFilterType)
                {
                    if(CYBLE_GLS_REC_NUM > seqNum1)
                    {
                        for(i = 0; i <= seqNum1; i++)
                        {
                            OpCodeOperation(i);
                        }
                    }
                }
                else if(CYBLE_GLS_RACP_OPD_2 == racpFilterType)
                {
                    if(CYBLE_TIME_GREAT == TimeComparison(glsGlucose[CYBLE_GLS_REC_NUM - 1].baseTime, userFacingTime1))
                    {
                        for(i = 0; i < CYBLE_GLS_REC_NUM; i++)
                        {
                            if(CYBLE_TIME_GREAT != TimeComparison(glsGlucose[i].baseTime, userFacingTime1))
                            {
                                OpCodeOperation(i);
                            }
                        }
                    }
                }
                else
                {
                    racpInd[3] = CYBLE_GLS_RACP_RSP_UNSPRT_OPD;
                }
                GlsInd();
                break;

            case CYBLE_GLS_RACP_OPR_GREAT:
                if(CYBLE_GLS_RACP_OPD_1 == racpFilterType)
                {
                    if(CYBLE_GLS_REC_NUM > seqNum1)
                    {
                        for(i = seqNum1; i < CYBLE_GLS_REC_NUM; i++)
                        {
                            OpCodeOperation(i);
                        }
                    }
                }
                else if(CYBLE_GLS_RACP_OPD_2 == racpFilterType)
                {
                    if(CYBLE_TIME_GREAT == TimeComparison(glsGlucose[CYBLE_GLS_REC_NUM - 1].baseTime, userFacingTime1))
                    {
                        for(i = 0; i < CYBLE_GLS_REC_NUM; i++)
                        {
                            if(CYBLE_TIME_LESS != TimeComparison(glsGlucose[i].baseTime, userFacingTime1))
                            {
                                OpCodeOperation(i);
                            }
                        }
                    }
                }
                else
                {
                    racpInd[3] = CYBLE_GLS_RACP_RSP_UNSPRT_OPD;
                }

                GlsInd();
                break;

            case CYBLE_GLS_RACP_OPR_WITHIN:
                if(CYBLE_GLS_RACP_OPD_1 == racpFilterType)
                {
                    if(seqNum2 >= CYBLE_GLS_REC_NUM)
                    {
                        seqNum2 = CYBLE_GLS_REC_NUM - 1;
                    }

                    if(seqNum1 > seqNum2)
                    {
                        racpInd[3] = CYBLE_GLS_RACP_RSP_INV_OPD;
                    }
                    else
                    {
                        for(i = seqNum1; i <= seqNum2; i++)
                        {
                            OpCodeOperation(i);
                        }
                    }
                }
                else if(CYBLE_GLS_RACP_OPD_2 == racpFilterType)
                {
                    if(CYBLE_TIME_GREAT != TimeComparison(glsGlucose[CYBLE_GLS_REC_NUM - 1].baseTime, userFacingTime2))
                    {
                        userFacingTime2 = glsGlucose[CYBLE_GLS_REC_NUM - 1].baseTime;
                    }

                    if(CYBLE_TIME_GREAT == TimeComparison(userFacingTime1, userFacingTime2))
                    {
                        racpInd[3] = CYBLE_GLS_RACP_RSP_INV_OPD;
                    }
                    else
                    {
                        for(i = 0; i < CYBLE_GLS_REC_NUM; i++)
                        {
                            if((CYBLE_TIME_LESS != TimeComparison(glsGlucose[i].baseTime, userFacingTime1)) &&
                               (CYBLE_TIME_GREAT != TimeComparison(glsGlucose[i].baseTime, userFacingTime2)))
                            {
                                OpCodeOperation(i);
                            }
                        }
                    }
                }
                else
                {
                    racpInd[3] = CYBLE_GLS_RACP_RSP_UNSPRT_OPD;
                }

                GlsInd();
                break;

            default:
                racpInd[3] = CYBLE_GLS_RACP_RSP_UNSPRT_OPR;
                GlsInd();
                break;
        }

        racpCommand = 0u;
    }
}
/* [] END OF FILE */
