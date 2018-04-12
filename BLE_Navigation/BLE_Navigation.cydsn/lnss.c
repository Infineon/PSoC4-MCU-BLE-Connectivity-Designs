/*******************************************************************************
* File Name: lnss.c
*
* Version 1.0
*
* Description:
*  This file contains the Location and Navigation Service related code.
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


uint8 cp[7u];
uint8 lnsFlag = 0u;
uint8 lnsTimer = 0u;
uint8 lnsCount = 0u;

CYBLE_LNS_LS_T ls;
CYBLE_LNS_NV_T nv;
CYBLE_DATE_TIME_T time = {2015u, 5u, 21u, 14u, 14u, 41u};


/* Location and Speed characteristic data */
const CYBLE_LNS_LS_T cls[LNS_COUNT] =
{
    {
        CYBLE_LNS_LS_FLG_LC |
        CYBLE_LNS_LS_FLG_EL |
        CYBLE_LNS_LS_FLG_UTC, /* Flags */
        138,       /* Instantaneous Speed */
        14u,       /* Total Distance */
        498090300, /* Location - Latitude */ 
        240413960, /* Location - Longitude */
        34409,     /* Elevation */ 
        13500,     /* Heading */
        1,         /* Rolling Time */
        {2015u, 5u, 21u, 14u, 14u, 41u}
    },
    {
        CYBLE_LNS_LS_FLG_IS |
        CYBLE_LNS_LS_FLG_TD |
        CYBLE_LNS_LS_FLG_LC |
        CYBLE_LNS_LS_FLG_HD |
        CYBLE_LNS_LS_FLG_RT, /* Flags */
        137,       /* Instantaneous Speed */
        14u,       /* Total Distance */
        498090200, /* Location - Latitude */
        240414060, /* Location - Longitude */
        34420,     /* Elevation */
        22500,     /* Heading */
        2,         /* Rolling Time */
        {2015u, 5u, 21u, 14u, 14u, 42u}
    },
    {
        CYBLE_LNS_LS_FLG_IS |
        CYBLE_LNS_LS_FLG_TD |
        CYBLE_LNS_LS_FLG_LC |
        CYBLE_LNS_LS_FLG_EL |
        CYBLE_LNS_LS_FLG_RT, /* Flags */
        139,       /* Instantaneous Speed */
        14u,       /* Total Distance */
        498090100, /* Location - Latitude */
        240413960, /* Location - Longitude */
        34431,     /* Elevation */
        31500,     /* Heading */
        3,         /* Rolling Time */
        {2015u, 5u, 21u, 14u, 14u, 43u}
    },
    {
        CYBLE_LNS_LS_FLG_TD |
        CYBLE_LNS_LS_FLG_LC |
        CYBLE_LNS_LS_FLG_EL |
        CYBLE_LNS_LS_FLG_HD |
        CYBLE_LNS_LS_FLG_RT, /* Flags */
        141,       /* Instantaneous Speed */
        14u,       /* Total Distance */
        498090200, /* Location - Latitude */
        240413860, /* Location - Longitude */
        34420,     /* Elevation */
        4500,      /* Heading */
        4,         /* Rolling Time */
        {2015u, 5u, 21u, 14u, 14u, 44u}
    }
};

/* Navigation characteristic data */
const CYBLE_LNS_NV_T cnv[LNS_COUNT] =
{
    {
        CYBLE_LNS_NV_FLG_RD | CYBLE_LNS_NV_FLG_RVD | CYBLE_LNS_NV_FLG_EAT,    /* Flags */
        9000,      /* Bearing */
        13500,     /* Heading */
        14u,       /* Remaining Distance */
        11,        /* Remaining Vertical Distance */
        {2015u, 5u, 21u, 14u, 14u, 41u}
    },
    {
        CYBLE_LNS_NV_FLG_RD | CYBLE_LNS_NV_FLG_RVD | CYBLE_LNS_NV_FLG_EAT,    /* Flags */
        9000,      /* Bearing */
        22500,     /* Heading */
        14u,       /* Remaining Distance */
        11,        /* Remaining Vertical Distance */
        {2015u, 5u, 21u, 14u, 14u, 42u}
    },
    {
        CYBLE_LNS_NV_FLG_RD | CYBLE_LNS_NV_FLG_RVD | CYBLE_LNS_NV_FLG_EAT,    /* Flags */
        9000,      /* Bearing */
        31500,     /* Heading */
        14u,       /* Remaining Distance */
        -11,       /* Remaining Vertical Distance */
        {2015u, 5u, 21u, 14u, 14u, 43u}
    },
    {
        CYBLE_LNS_NV_FLG_RD | CYBLE_LNS_NV_FLG_RVD | CYBLE_LNS_NV_FLG_EAT,    /* Flags */
        9000,      /* Bearing */
        4500,      /* Heading */
        14u,       /* Remaining Distance */
        -11,       /* Remaining Vertical Distance */
        {2015u, 5u, 21u, 14u, 14u, 44u}
    }
};


/*******************************************************************************
* Function Name: LnsCallBack
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service-specific events for
*   Location and Navigation Service.
*
* Parameters:
*  event - The event code.
*  *eventParam - The pointer to event data.
*
* Return:
*  None.
*
********************************************************************************/
void LnsCallBack(uint32 event, void* eventParam)
{
    switch(event)
    {
        case CYBLE_EVT_LNSS_WRITE_CHAR:
            DBG_PRINTF("CP is written: ");
            for(i = 0; i < ((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->value->len; i++)
            {
                DBG_PRINTF("%2.2x ", ((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->value->val[i]);
            }
            DBG_PRINTF("\r\n");
            
            cp[2u] = CYBLE_LNS_CP_RSP_SUCCESS;
            
            DBG_PRINTF("Opcode: ");
            cp[1u] = ((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->value->val[0];
            switch(cp[1u])
            {
                case CYBLE_LNS_CP_OPC_SCV:
                    DBG_PRINTF("Set Cumulative Value \r\n");
                    if(4u == ((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->value->len)
                    {
                        ls.totalDst = (uint32)((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->value->val[1];
                        ls.totalDst |= (uint32)((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->value->val[2] << 8;
                        ls.totalDst |= (uint32)((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->value->val[3] << 16;
                    }
                    else
                    {
                        cp[2u] = CYBLE_LNS_CP_RSP_INV_PAR;
                        DBG_PRINTF("Invalid Parameter \r\n");
                    }
                    break;
                    
                case CYBLE_LNS_CP_OPC_MLS:
                    DBG_PRINTF("Mask Location and Speed \r\n");
                    break;
                    
                case CYBLE_LNS_CP_OPC_NC:
                    DBG_PRINTF("Navigation Control\r\nParameter: ");
                    switch(((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->value->val[1])
                    {
                        case CYBLE_LNS_CP_OPC_NC_STOP:
                            DBG_PRINTF("Stop navigation \r\n");
                            lnsFlag &= ~NV_EN;
                            Navigation_LED_Write(LED_OFF);
                            break;
                            
                        case CYBLE_LNS_CP_OPC_NC_START:
                            DBG_PRINTF("Start navigation \r\n");
                            lnsFlag |= NV_EN;
                            led = LED_ON;
                            Navigation_LED_Write(LED_ON);
                            break;
                            
                        case CYBLE_LNS_CP_OPC_NC_PAUSE:
                            DBG_PRINTF("Pause navigation \r\n");
                            lnsFlag &= ~NV_EN;
                            led = LED_ON;
                            Navigation_LED_Write(LED_ON);
                            break;  
                            
                        case CYBLE_LNS_CP_OPC_NC_RESUME:
                            DBG_PRINTF("Resume navigation \r\n");
                            lnsFlag |= NV_EN;
                            led = LED_ON;
                            Navigation_LED_Write(LED_ON);
                            break;
                            
                        case CYBLE_LNS_CP_OPC_NC_SKIP:
                            DBG_PRINTF("Skip waypoint \r\n");
                            break;
                            
                        case CYBLE_LNS_CP_OPC_NC_NEAR:
                            DBG_PRINTF("Start navigation to nearest waypoint \r\n");
                            lnsFlag |= NV_EN;
                            break;
                            
                        default:
                            DBG_PRINTF("Unknown \r\n");
                            break;
                    }       
                    break;
                    
                case CYBLE_LNS_CP_OPC_NRS:
                    DBG_PRINTF("Request Number of Routes \r\n");
                    break;
                    
                case CYBLE_LNS_CP_OPC_RNM:
                    DBG_PRINTF("Request Name of Route \r\n");
                    break;
                    
                case CYBLE_LNS_CP_OPC_SR:
                    DBG_PRINTF("Select Route \r\n");
                    break;
                    
                case CYBLE_LNS_CP_OPC_SFR:
                    DBG_PRINTF("Set Fix Rate \r\n");
                    break;
                    
                case CYBLE_LNS_CP_OPC_SE:
                    DBG_PRINTF("Set Elevation \r\n");
                    ls.elevation = (uint32)((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->value->val[1];
                    ls.elevation |= (uint32)((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->value->val[2] << 8;
                    ls.elevation |= (uint32)((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->value->val[3] << 16;
                    break;
                    
                default:
                    DBG_PRINTF("Unknown \r\n");
                    break;
            }
            break;
        
        case CYBLE_EVT_LNSS_NOTIFICATION_ENABLED:
            if(CYBLE_LNS_LS == ((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->charIndex)
            {
                DBG_PRINTF("Location and Speed Notification is Enabled \r\n");
                lnsFlag |= LS_NTF;
            }
            else
            {
                DBG_PRINTF("Navigation Notification is Enabled \r\n");
                lnsFlag |= NV_NTF;
            }
            break;
                
        case CYBLE_EVT_LNSS_NOTIFICATION_DISABLED:
            if(CYBLE_LNS_LS == ((CYBLE_LNS_CHAR_VALUE_T *)eventParam)->charIndex)
            {
                DBG_PRINTF("Location and Speed Notification is Disabled \r\n");
                lnsFlag &= ~LS_NTF;
            }
            else
            {
                DBG_PRINTF("Navigation Notification is Disabled \r\n");
                lnsFlag &= ~NV_NTF;
            }
            break;
            
        case CYBLE_EVT_LNSS_INDICATION_ENABLED:
            DBG_PRINTF("LN Control Point Indication is Enabled \r\n");
            break;
                
        case CYBLE_EVT_LNSS_INDICATION_DISABLED:
            DBG_PRINTF("LN Control Point Indication is Disabled \r\n");
            break;
            
        case CYBLE_EVT_LNSS_INDICATION_CONFIRMED:
            DBG_PRINTF("LN Control Point Indication is Confirmed \r\n");
            break;

		default:
            DBG_PRINTF("unknown LNS event: %lx \r\n", event);
			break;
    }
}


/*******************************************************************************
* Function Name: LnsInit
********************************************************************************
*
* Summary:
*   Initializes the Location and Navigation Service.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void LnsInit(void)
{
    cp[0u] = CYBLE_LNS_CP_OPC_RC;
    CyBle_LnsRegisterAttrCallback(LnsCallBack);
}


/*******************************************************************************
* Function Name: LnsInit
********************************************************************************
*
* Summary:
*   Sends the configured (allowed) notifications 
*   of the Location and Navigation Service.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void LnsNtf(void)
{
    if((lnsFlag & NV_EN) != 0u)
    {
        led ^= LED_OFF;
        Navigation_LED_Write(led);
    }
    
    lnsTimer++;
    if(lnsTimer >= LNS_TIME)
    {
        lnsTimer = 0u;
        lnsCount++;
        if(lnsCount >= LNS_COUNT)
        {
            lnsCount = 0u;
        }
        
        ls = cls[lnsCount];
        ls.rollTime++;
        ls.utcTime = time;
        
        if(0u != (lnsFlag & LS_NTF))
        {
            uint8 pdu[sizeof(CYBLE_LNS_LS_T)];
            uint8 ptr;
            
            /* Flags field is always first two bytes */
        	CyBle_Set16ByPtr(&pdu[0u], ls.flags);

            /* Next possible value is at 2nd byte */
            ptr = 2u;
                
            /* If Instantaneous Speed Present flag is set */
            if(0u != (ls.flags & CYBLE_LNS_LS_FLG_IS))
            {
                /* And set 2-byte Instantaneous Speed value */
                CyBle_Set16ByPtr(&pdu[ptr], ls.instSpd);
                /* Next data will be located beginning from 3rd byte */
                ptr += sizeof(ls.instSpd);
            }
            
            if(0u != (ls.flags & CYBLE_LNS_LS_FLG_TD))
            {
                pdu[ptr] = (uint8) (ls.totalDst & 0x000000FFu);
                pdu[ptr + 1] = (uint8) ((ls.totalDst & 0x0000FF00u) >> 8u);
                pdu[ptr + 2] = (uint8) ((ls.totalDst & 0x00FF0000u) >> 16u);
                ptr += 3; /* Total Distance size is actually 3 bytes */
            }

            if(0u != (ls.flags & CYBLE_LNS_LS_FLG_LC))
            {
                pdu[ptr] = (uint8) (ls.latitude & 0x000000FFu);
                pdu[ptr + 1] = (uint8) ((ls.latitude & 0x0000FF00u) >> 8u);
                pdu[ptr + 2] = (uint8) ((ls.latitude & 0x00FF0000u) >> 16u);
                pdu[ptr + 3] = (uint8) ((ls.latitude & 0xFF000000u) >> 24u);
                ptr += sizeof(ls.latitude);
                pdu[ptr] = (uint8) (ls.longitude & 0x000000FFu);
                pdu[ptr + 1] = (uint8) ((ls.longitude & 0x0000FF00u) >> 8u);
                pdu[ptr + 2] = (uint8) ((ls.longitude & 0x00FF0000u) >> 16u);
                pdu[ptr + 3] = (uint8) ((ls.longitude & 0xFF000000u) >> 24u);
                ptr += sizeof(ls.longitude);
            }
            
            if(0u != (ls.flags & CYBLE_LNS_LS_FLG_EL))
            {
                pdu[ptr] = (uint8) (ls.elevation & 0x000000FFu);
                pdu[ptr + 1] = (uint8) ((ls.elevation & 0x0000FF00u) >> 8u);
                pdu[ptr + 2] = (uint8) ((ls.elevation & 0x00FF0000u) >> 16u);
                ptr += 3; /* Elevation size is actually 3 bytes */
            }
            
            if(0u != (ls.flags & CYBLE_LNS_LS_FLG_HD))
            {
                CyBle_Set16ByPtr(&pdu[ptr], ls.heading);
                ptr += sizeof(ls.heading);
            }
            
            if(0u != (ls.flags & CYBLE_LNS_LS_FLG_RT))
            {
                ls.rollTime++;
                pdu[ptr] = ls.rollTime;
                ptr += sizeof(ls.rollTime);
            }
            
            if(0u != (ls.flags & CYBLE_LNS_LS_FLG_UTC))
            {
                CyBle_Set16ByPtr(&pdu[ptr], ls.utcTime.year);
                pdu[ptr + 2u] = ls.utcTime.month;
                pdu[ptr + 3u] = ls.utcTime.day;
                pdu[ptr + 4u] = ls.utcTime.hours;
                pdu[ptr + 5u] = ls.utcTime.minutes;
                pdu[ptr + 6u] = ls.utcTime.seconds;
                ptr += 7u;
            }
            
            do
            {
                CyBle_ProcessEvents();
            }
            while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);

            apiResult = CyBle_LnssSendNotification(cyBle_connHandle, CYBLE_LNS_LS, ptr, pdu);
            
        	if(apiResult != CYBLE_ERROR_OK)
        	{
        		DBG_PRINTF("CyBle_LnssSendNotification API (L&S) Error: ");
                PrintApiResult();
        	}
            else
            {
                DBG_PRINTF("L&S Ntf: ");
                for(i = 0; i < ptr; i++)
                {
                    DBG_PRINTF("%2.2x ", pdu[i]);
                }
                DBG_PRINTF("\r\n");
            }
        }
        
        time.seconds++;
        if(time.seconds >= 60u)
        {
            time.seconds = 0u;
            time.minutes++;
            if(time.minutes >= 60u)
            {
                time.minutes = 0u;
                time.hours++;
                if(time.hours >= 24u)
                {
                    time.hours = 0u;
                    time.day++;
                    /* Omit further data process because it is not critical for simulation example */
                }
            }
        }
        
        nv = cnv[lnsCount];
        nv.eaTime = time;
        
        if((lnsFlag & (NV_NTF | NV_EN)) == (NV_NTF | NV_EN))
        {
            uint8 pdu[sizeof(CYBLE_LNS_NV_T)];
            uint8 ptr;
            
            /* Flags field is always first two bytes */
        	CyBle_Set16ByPtr(&pdu[0u], nv.flags);
            
            /* Bearing mandatory field is next */
        	CyBle_Set16ByPtr(&pdu[2u], nv.bearing);
            
            /* Heading mandatory field is next */
        	CyBle_Set16ByPtr(&pdu[4u], nv.heading);

            /* Next possible value is at 6th byte */
            ptr = 6u;
                
            /* If Remaining Distance Present flag is set */
            if(0u != (nv.flags & CYBLE_LNS_NV_FLG_RD))
            {
                /* And set 2-byte Instantaneous Speed value */
                pdu[ptr] = (uint8) (nv.rDst & 0x000000FFu);
                pdu[ptr + 1] = (uint8) ((nv.rDst & 0x0000FF00u) >> 8u);
                pdu[ptr + 2] = (uint8) ((nv.rDst & 0x00FF0000u) >> 16u);
                ptr += 3; /* Remaining Distance size is actually 3 bytes */
            }
            
            if(0u != (nv.flags & CYBLE_LNS_NV_FLG_RVD))
            {
                pdu[ptr] = (uint8) (nv.rvDst & 0x000000FFu);
                pdu[ptr + 1] = (uint8) ((nv.rvDst & 0x0000FF00u) >> 8u);
                pdu[ptr + 2] = (uint8) ((nv.rvDst & 0x00FF0000u) >> 16u);
                ptr += 3; /* Remaining Vertical Distance size is actually 3 bytes */
            }

            if(0u != (nv.flags & CYBLE_LNS_NV_FLG_EAT))
            {
                CyBle_Set16ByPtr(&pdu[ptr], nv.eaTime.year);
                pdu[ptr + 2u] = nv.eaTime.month;
                pdu[ptr + 3u] = nv.eaTime.day;
                pdu[ptr + 4u] = nv.eaTime.hours;
                pdu[ptr + 5u] = nv.eaTime.minutes;
                pdu[ptr + 6u] = nv.eaTime.seconds;
                ptr += 7u;
            }
            
            do
            {
                CyBle_ProcessEvents();
            }
            while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);

            apiResult = CyBle_LnssSendNotification(cyBle_connHandle, CYBLE_LNS_NV, ptr, pdu);
            
        	if(apiResult != CYBLE_ERROR_OK)
        	{
                DBG_PRINTF("CyBle_LnssSendNotification API (Navigation) Error: ");
                PrintApiResult();
        	}
            else
            {
                DBG_PRINTF("Navigation Ntf: ");
                for(i = 0; i < ptr; i++)
                {
                    DBG_PRINTF("%2.2x ", pdu[i]);
                }
                DBG_PRINTF("\r\n");
            }
        }
    }
}


/*******************************************************************************
* Function Name: LnsProcess
********************************************************************************
*
* Summary:
*   Processes the LNS functionality in the main loop.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void LnsProcess(void)
{
    if(cp[2u] != 0u)
    {
        uint8 cpSize = 3u;
        
        switch(cp[1u])
        {
            case CYBLE_LNS_CP_OPC_SCV:
                break;
                
            case CYBLE_LNS_CP_OPC_MLS:
                break;
                
            case CYBLE_LNS_CP_OPC_NC:       
                break;
                
            case CYBLE_LNS_CP_OPC_NRS:
                cp[3u] = LO8(LNS_COUNT);
                cp[4u] = HI8(LNS_COUNT);
                cpSize = 5u;
                break;
                
            case CYBLE_LNS_CP_OPC_RNM:
                cp[3u] = 'N';
                cp[4u] = 'A';
                cp[5u] = 'M';
                cp[6u] = 'E';
                cpSize = 7u;
                break;
                
            case CYBLE_LNS_CP_OPC_SR:
                break;
                
            case CYBLE_LNS_CP_OPC_SFR:
                break;
                
            case CYBLE_LNS_CP_OPC_SE:
                break;
                
            default:
                cp[2u] = CYBLE_LNS_CP_RSP_UNSPRT_OPC;
                break;
        }
    
        do
        {
            CyBle_ProcessEvents();
        }
        while(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY);

        if(CYBLE_ERROR_OK != (apiResult = CyBle_LnssSendIndication(cyBle_connHandle, CYBLE_LNS_CP, cpSize, cp)))
        {
            DBG_PRINTF("CyBle_LnssSendIndication API Error: %x \r\n", apiResult);
        }
        else
        {
            DBG_PRINTF("CP Ind:");
            for(i = 0; i < cpSize; i++)
            {
                DBG_PRINTF(" %2.2x", cp[i]);
            }
            DBG_PRINTF("\r\n");
        }
        
        cp[2u] = 0u;
    }
}

/* [] END OF FILE */
