/*******************************************************************************
* File Name: blss.c
*
* Version 1.0
*
* Description:
*  This file contains BLS service related code.
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
uint8 blsSim; /* Blood Pressure Measurement simulation counter */
uint16 feature = CYBLE_BLS_BPF_CFD | CYBLE_BLS_BPF_PRD | CYBLE_BLS_BPF_MBS;


/* Blood Pressure Measurement values */
CYBLE_BLS_BPM_T blsBpm[] =
{
    {
        CYBLE_BLS_BPM_FLG_TSP | CYBLE_BLS_BPM_FLG_PRT | CYBLE_BLS_BPM_FLG_UID | CYBLE_BLS_BPM_FLG_MST,
        0x008au /* Systolic 138.0 mmHg */,
        0x004fu /* Diastolic 79.0 mmHg */,
        0x0050u /* MAP 80.0 mmHg */,
        {2014u, 9u, 8u, 13u, 20u, 45u},
        0xf321u /* 80.1 */,
        1u,
        CYBLE_BLS_BPM_MST_BMD
    }
};

/* Intermediate Cuff Pressure values */
CYBLE_BLS_BPM_T blsIcp[] =
{
    {   CYBLE_BLS_BPM_FLG_TSP | CYBLE_BLS_BPM_FLG_PRT | CYBLE_BLS_BPM_FLG_UID | CYBLE_BLS_BPM_FLG_MST,
        0x0091u /* 145.0 mmHg */,
        0x07ffu /* NaN */,
        0x07ffu /* NaN */,
        {2014u, 9u, 8u, 13u, 20u, 40u},
        0xf33au /* 82.6 */,
        1u,
        CYBLE_BLS_BPM_MST_BMD
    }
};



/*******************************************************************************
* Function Name: BlsCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service specific events from
*   Blood Pressure Service.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
* Return:
*  None.
*
********************************************************************************/
void BlsCallBack(uint32 event, void* eventParam)
{
    if(0u != eventParam)
    {
        /* This dummy operation is to avoid warning about unused eventParam */
    }

    switch(event)
    {
        case CYBLE_EVT_BLSS_NOTIFICATION_ENABLED:
            DBG_PRINTF("Intermediate Cuff Pressure Notification is Enabled \r\n");
            blsSim = 0u;
            break;

        case CYBLE_EVT_BLSS_NOTIFICATION_DISABLED:
            DBG_PRINTF("Intermediate Cuff Pressure Notification is Disabled \r\n");
            break;

        case CYBLE_EVT_BLSS_INDICATION_ENABLED:
            DBG_PRINTF("Blood Pressure Measurement Indication is Enabled \r\n");
            blsSim = 0u;
            break;

        case CYBLE_EVT_BLSS_INDICATION_DISABLED:
            DBG_PRINTF("Blood Pressure Measurement Indication is Disabled \r\n");
            break;

        case CYBLE_EVT_BLSS_INDICATION_CONFIRMED:
            DBG_PRINTF("Blood Pressure Measurement Indication is Confirmed \r\n");
            break;

        default:
            DBG_PRINTF("unknown BLS event: %lx \r\n", event);
            break;
    }
}

/*******************************************************************************
* Function Name: BlsInit()
********************************************************************************
*
* Summary:
*   Initializes the blood pressure service.
*
*******************************************************************************/
void BlsInit(void)
{
    CyBle_BlsRegisterAttrCallback(BlsCallBack);
}


/*******************************************************************************
* Function Name: BlsInd
********************************************************************************
*
* Summary:
*   Sends the Blood Pressure Measurement indication.
*
* Parameters:
*   uint8 num - number of record to notify.
*
* Return:
*   None
*
*******************************************************************************/
void BlsInd(uint8 num)
{
    uint16 cccd;
        
    (void) CyBle_BlssGetCharacteristicDescriptor(CYBLE_BLS_BPM, CYBLE_BLS_CCCD, CYBLE_CCCD_LEN, (uint8*)&cccd);
                                                        
    if(cccd == CYBLE_CCCD_INDICATION)
    {
    
        uint8 pdu[sizeof(CYBLE_BLS_BPM_T)];
        uint8 ptr;

        /* flags, Systolic, Diastolic and Mean Arterial Pressure fields always go first */
        pdu[0u] = blsBpm[num].flags;
        pdu[1u] = LO8(blsBpm[num].sys);
        pdu[2u] = HI8(blsBpm[num].sys);
        pdu[3u] = LO8(blsBpm[num].dia);
        pdu[4u] = HI8(blsBpm[num].dia);
        pdu[5u] = LO8(blsBpm[num].map);
        pdu[6u] = HI8(blsBpm[num].map);
        
        /* if the Time Stamp Present flag is set */
        if(0u != (blsBpm[num].flags & CYBLE_BLS_BPM_FLG_TSP))
        {
            /* set the full 7-bytes Time Stamp value */
            pdu[7u] = LO8(blsBpm[num].time.year);
            pdu[8u] = HI8(blsBpm[num].time.year);
            pdu[9u] = blsBpm[num].time.month;
            pdu[10u] = blsBpm[num].time.day;
            pdu[11u] = blsBpm[num].time.hours;
            pdu[12u] = blsBpm[num].time.minutes;
            pdu[13u] = blsBpm[num].time.seconds;

            /* the next data will be located at 14th byte */
            ptr = 14u;
        }
        else
        {
            /* the next data will be located at 7th byte */
            ptr = 7u;   
        }

        if(0u != (blsBpm[num].flags & CYBLE_BLS_BPM_FLG_PRT))
        {
            pdu[ptr] = LO8(blsBpm[num].prt);
            pdu[ptr + 1u] = HI8(blsBpm[num].prt);
            ptr += 2u;
        }

        if(0u != (blsBpm[num].flags & CYBLE_BLS_BPM_FLG_UID))
        {
            pdu[ptr] = blsBpm[num].uid;
            ptr += 1u;
        }

        if(0u != (blsBpm[num].flags & CYBLE_BLS_BPM_FLG_MST))
        {
            pdu[ptr] = LO8(blsBpm[num].mst);
            pdu[ptr + 1u] = HI8(blsBpm[num].mst);
            ptr += 2u;
        }
        
        do
        {
            CyBle_ProcessEvents();
        }
        while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);

        if(CYBLE_ERROR_OK != (apiResult = CyBle_BlssSendIndication(cyBle_connHandle, CYBLE_BLS_BPM, ptr, pdu)))
        {
            DBG_PRINTF("CyBle_GlssSendNotification API Error: ");
            PrintApiResult();
        }
        else
        {
            DBG_PRINTF("Blood Pressure Ind  sys:%d mmHg, dia:%d mmHg\r\n", blsBpm[num].sys, blsBpm[num].dia);
        }
    }
}


/*******************************************************************************
* Function Name: BlsNtf
********************************************************************************
*
* Summary:
*   Sends the BLS Intermediate Cuff Pressure notification.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void BlsNtf(uint8 num)
{
    uint16 cccd;
        
    (void) CyBle_BlssGetCharacteristicDescriptor(CYBLE_BLS_ICP, CYBLE_BLS_CCCD, CYBLE_CCCD_LEN, (uint8*)&cccd);
                                                        
    if(cccd == CYBLE_CCCD_NOTIFICATION)
    {
        uint8 pdu[sizeof(CYBLE_BLS_BPM_T)];
        uint8 ptr;
        
        /* flags, Systolic, Diastolic and Mean Arterial Pressure fields always go first */
        pdu[0u] = blsIcp[num].flags;
        pdu[1u] = LO8(blsIcp[num].sys);
        pdu[2u] = HI8(blsIcp[num].sys);
        pdu[3u] = LO8(blsIcp[num].dia);
        pdu[4u] = HI8(blsIcp[num].dia);
        pdu[5u] = LO8(blsIcp[num].map);
        pdu[6u] = HI8(blsIcp[num].map);

        /* if the Time Stamp Present flag is set */
        if(0u != (blsIcp[num].flags & CYBLE_BLS_BPM_FLG_TSP))
        {
            /* set the full 7-bytes Time Stamp value */
            pdu[7u] = LO8(blsIcp[num].time.year);
            pdu[8u] = HI8(blsIcp[num].time.year);
            pdu[9u] = blsIcp[num].time.month;
            pdu[10u] = blsIcp[num].time.day;
            pdu[11u] = blsIcp[num].time.hours;
            pdu[12u] = blsIcp[num].time.minutes;
            pdu[13u] = blsIcp[num].time.seconds;

            /* the next data will be located at 14th byte */
            ptr = 14u;
        }
        else
        {
            /* the next data will be located at 7th byte */
            ptr = 7u;   
        }

        if(0u != (blsIcp[num].flags & CYBLE_BLS_BPM_FLG_PRT))
        {
            pdu[ptr] = LO8(blsIcp[num].prt);
            pdu[ptr + 1u] = HI8(blsIcp[num].prt);
            ptr += 2u;
        }

        if(0u != (blsIcp[num].flags & CYBLE_BLS_BPM_FLG_UID))
        {
            pdu[ptr] = blsIcp[num].uid;
            ptr += 1u;
        }

        if(0u != (blsIcp[num].flags & CYBLE_BLS_BPM_FLG_MST))
        {
            pdu[ptr] = LO8(blsIcp[num].mst);
            pdu[ptr + 1u] = HI8(blsIcp[num].mst);
            ptr += 2u;
        }

        if(CYBLE_ERROR_OK != (apiResult = CyBle_BlssSendNotification(cyBle_connHandle, CYBLE_BLS_ICP, ptr, pdu)))
        {
            DBG_PRINTF("CyBle_BlssSendNotification API Error: ");
            PrintApiResult();
        }
        else
        {
            DBG_PRINTF("Intermediate Cuff Pressure Ntf: %d mmHg\r\n", blsIcp[num].sys);
        }
    }
}


/*******************************************************************************
* Function Name: BlsProcess
********************************************************************************
*
* Summary:
*   Executes current BLS processes.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void BlsSimulate(void)
{
    static uint32 blsTimer = BLS_TIMEOUT;
    
    if(--blsTimer == 0u) 
    {
        blsTimer = BLS_TIMEOUT;
    
        blsSim++;
        if(blsSim > SIM_UNIT_MAX)
        {
            blsSim = 0;
        }
        
        blsIcp[0u].sys = SIM_ICF_MAX - SIM_BLS_DLT * (blsSim & SIM_ICF_MSK);
        blsIcp[0u].time.seconds = blsSim;
        BlsNtf(0u);
        
        blsBpm[0u].sys = SIM_BPM_SYS_MIN + (blsSim & SIM_BPM_MSK);
        blsBpm[0u].dia = SIM_BPM_DIA_MIN + (blsSim & SIM_BPM_MSK);
        blsBpm[0u].time.seconds = blsSim;
        BlsInd(0u);
    }
}

/* [] END OF FILE */
