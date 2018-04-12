/*******************************************************************************
* File Name: ancsc.c
*
* Version 1.0
*
* Description:
*  This file contains the Apple Notification Center Service related code.
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

#include "ancsc.h"


/* Global variables */
uint8 ancsFlag = 0u; /* ANCS specific flags */
uint8 nsCnt = 0u; /* Count of the notifications */
CYBLE_ANCS_NS_T ns[CYBLE_ANCS_NS_CNT]; /* array of the recieved notifications */
CYBLE_ANCS_CP_T cp; /* Control point value */
CYBLE_ANCS_DS_T ds; /* Data source value */


/*******************************************************************************
* Function Name: AncsPrintCharName
********************************************************************************
*
* Summary:
*   Decodes and prints the Apple Notification Center service characteristic name.
*
* Parameters:
*  charIndex - ANCS characteristic index.
*
* Return:
*  None.
*
********************************************************************************/
void AncsPrintCharName(CYBLE_ANCS_CHAR_INDEX_T charIndex)
{
    switch(charIndex)
    {
        case CYBLE_ANCS_NS:                    
            printf("Notification Source");
            break;
        
        case CYBLE_ANCS_CP:                    
            printf("Control Point");
            break;
            
        case CYBLE_ANCS_DS:
            printf("Data Source");
            break;
            
        default:
            printf("Unknown ANCS");
            break;
    }
    
    printf(" characteristic ");
}


/*******************************************************************************
* Function Name: AncsInit
********************************************************************************
*
* Summary:
*   Initializes the Apple Notification Center Service.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void AncsInit(void)
{
    CyBle_AncsRegisterAttrCallback(AncsCallBack);
}

/*******************************************************************************
* Function Name: AncsNextAct
********************************************************************************
*
* Summary:
*   Initiates the next action processing.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void AncsNextAct(void)
{
    printf("\r\n\n");
    
    switch(cp.attId)
    {
        case CYBLE_ANCS_CP_ATT_ID_TTL: /* If previous Attribute ID is "Title" */
            switch(cp.ctgId)
            {
                case CYBLE_ANCS_NS_CAT_ID_EML: /* If Category ID is "Email" */
                    cp.attId = CYBLE_ANCS_CP_ATT_ID_SBT; /* Next Attribute ID is "Subtitle" */
                    ancsFlag |= CYBLE_ANCS_FLG_CMD; /* Trig control point command write */
                    break;
                
                case CYBLE_ANCS_NS_CAT_ID_SOC: /* If Category ID is "Social" */
                    cp.attId = CYBLE_ANCS_CP_ATT_ID_MSG; /* Next Attribute ID is "Message" */
                    ancsFlag |= CYBLE_ANCS_FLG_CMD; /* Trig control point command write */
                    break;
                
                case CYBLE_ANCS_NS_CAT_ID_INC: /* If Category ID is "Incoming Call" */
                    if(((ns[nsCnt].evtFlg & CYBLE_ANCS_NS_FLG_PA) != 0u) &&
                       ((ns[nsCnt].evtFlg & CYBLE_ANCS_NS_FLG_NA) != 0u))
                    { /* If "Positive Action" and "Negative Action" flags are set */
                        printf("User Action: Accept  (two pressing SW2 per second)\r\n");
                        printf("             Decline (one pressing SW2 per second)? : ");
                        ancsFlag |= CYBLE_ANCS_FLG_ACT; /* Trig polling/waiting for user action */
                    }
                    break;
                
                default:
                    break;
            }
            break;
        
        case CYBLE_ANCS_CP_ATT_ID_SBT: /* If previous Attribute ID is "Subtitle" */
            if(cp.ctgId == CYBLE_ANCS_NS_CAT_ID_EML) /* If Category ID is "Email" */
            {
                cp.attId = CYBLE_ANCS_CP_ATT_ID_MSG; /* Next Attribute ID is "Message" */
                ancsFlag |= CYBLE_ANCS_FLG_CMD; /* Trig control point command write */
            }
            break;
            
        case CYBLE_ANCS_CP_ATT_ID_MSG: /* If previous Attribute ID is "Message" */
            switch(cp.ctgId)
            {
                case CYBLE_ANCS_NS_CAT_ID_EML: /* If Category ID is "Email" */
                    ancsFlag &= (uint8)~CYBLE_ANCS_FLG_NTF; /* Stop to process current Notification */
                    break;
                
                case CYBLE_ANCS_NS_CAT_ID_SOC: /* If Category ID is "social" */
                    if((ns[nsCnt].evtFlg & CYBLE_ANCS_NS_FLG_NA) != 0u)
                    { /* If "negative action" flag is set */
                        printf("User Action: Decline (one pressing SW2 per second)? : ");
                        ancsFlag |= CYBLE_ANCS_FLG_ACT; /* Trig polling/waiting for user action */
                    }
                    break;
                
                default:
                    break;
            }
            break;
            
        default:
            ancsFlag &= (uint8)~CYBLE_ANCS_FLG_NTF; /* Stop to process current Notification */
            break;
    }
}


/*******************************************************************************
* Function Name: AncsCallBack
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service-specific events for
*   Apple Notification Center Service.
*
* Parameters:
*  event - The event code.
*  *eventParam - The pointer to the event data.
*
* Return:
*  None.
*
********************************************************************************/
void AncsCallBack(uint32 event, void* eventParam)
{    
    switch(event)
    {
        case CYBLE_EVT_ANCSC_NOTIFICATION:   
            switch(((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->charIndex)
            {
                case CYBLE_ANCS_NS:
                    {
                        uint8 locFlag = 0u;
                        
                        /* Unpack the Notification ID */
                        uint32 locNtfUid = ((uint32)((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->value->val[4u]) +
                                         ((uint32)((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->value->val[5u] << 8u) +
                                         ((uint32)((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->value->val[6u] << 16u) +
                                         ((uint32)((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->value->val[7u] << 24u);
                                      
                        if((ancsFlag & CYBLE_ANCS_FLG_ACT) != 0u)
                        {
                            /* Discard pending User Action and current notification processing */
                            ancsFlag &= (uint8)~(CYBLE_ANCS_FLG_ACT | CYBLE_ANCS_FLG_NTF);
                            printf("User Action is discarded \r\n");
                        }
                        
                        /* Check if this Notification ID is already stored */
                        for(i = 0u; i < nsCnt; i++)
                        {
                            if(locNtfUid == ns[i].ntfUid)
                            {
                                locFlag = 1u;
                            }
                        }
                        
                        if(locFlag == 0u) /* If it is "new" Notification */
                        {   
                            /* Unpack all Notification Source characteristic fields */
                            ns[nsCnt].evtId = (CYBLE_ANCS_NS_EVT_ID_T)((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->value->val[0u];
                            ns[nsCnt].evtFlg = ((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->value->val[1u];
                            ns[nsCnt].ctgId = (CYBLE_ANCS_NS_CAT_ID_T)((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->value->val[2u];
                            ns[nsCnt].ctgCnt = ((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->value->val[3u];
                            ns[nsCnt].ntfUid = locNtfUid;
                            
                            /* Print all details of Notification */
                            printf("\r\nEventID: ");
                            switch(ns[nsCnt].evtId)
                            {
                                case CYBLE_ANCS_NS_EVT_ID_ADD:
                                    printf("Notification Added\r\n");
                                    break;
                                
                                case CYBLE_ANCS_NS_EVT_ID_MOD:
                                    printf("Notification Modified\r\n");
                                    break;
                                
                                case CYBLE_ANCS_NS_EVT_ID_REM:
                                    printf("Notification Removed\r\n");
                                    break;
                                
                                default:
                                    printf("Unsupported\r\n");
                                    break;
                            }
                            
                            printf("EventFlags: ");
                            if((ns[nsCnt].evtFlg & CYBLE_ANCS_NS_FLG_SL) != 0u)
                            {
                                printf("Silent, ");
                            }
                            
                            if((ns[nsCnt].evtFlg & CYBLE_ANCS_NS_FLG_IM) != 0u)
                            {
                                printf("Important, ");
                            }
                            
                            if((ns[nsCnt].evtFlg & CYBLE_ANCS_NS_FLG_PE) != 0u)
                            {
                                printf("Pre-existing, ");
                            }
                            
                            if((ns[nsCnt].evtFlg & CYBLE_ANCS_NS_FLG_PA) != 0u)
                            {
                                printf("Positive Action, ");
                            }
                            
                            if((ns[nsCnt].evtFlg & CYBLE_ANCS_NS_FLG_NA) != 0u)
                            {
                                printf("Negative Action, ");
                            }
                            printf("\r\n");
                            
                            printf("CategoryCount: %d\r\n", ns[nsCnt].ctgCnt);
                            printf("NotificationUID: 0x%8.8lx\r\n", ns[nsCnt].ntfUid);
                            
                            printf("CategoryID: ");
                            switch(ns[nsCnt].ctgId)
                            {
                                case CYBLE_ANCS_NS_CAT_ID_OTH: /* If Category ID is "Other" */
                                    printf("Other\r\n");
                                    break;
                                    
                                case CYBLE_ANCS_NS_CAT_ID_INC: /* If Category ID is "Incoming Call" */
                                    printf("Incoming Call\r\n");
                                    if(ns[nsCnt].evtId == CYBLE_ANCS_NS_EVT_ID_ADD) /* If Event ID is "Notification Added" */
                                    { 
                                        if(nsCnt < (CYBLE_ANCS_NS_CNT - 1)) /* If notification queue is not full */
                                        {
                                            nsCnt++; /* Increment notification counter */
                                        }
                                    }
                                    break;
                                
                                case CYBLE_ANCS_NS_CAT_ID_MIS: /* If Category ID is "Missed Call" */
                                    printf("Missed Call\r\n");
                                    break;
                                
                                case CYBLE_ANCS_NS_CAT_ID_VML: /* If Category ID is "Voice mail" */
                                    printf("Voicemail\r\n");
                                    break;
                                
                                case CYBLE_ANCS_NS_CAT_ID_SOC: /* If Category ID is "Social" */
                                    printf("Social\r\n");
                                    if(ns[nsCnt].evtId == CYBLE_ANCS_NS_EVT_ID_ADD) /* If Event ID is "Notification Added" */
                                    {
                                        if(nsCnt < (CYBLE_ANCS_NS_CNT - 1)) /* If notification queue is not full */
                                        {
                                            nsCnt++; /* Increment notification counter */
                                        }
                                    }
                                    break;
                                
                                case CYBLE_ANCS_NS_CAT_ID_SCH: /* If the Category ID is "Schedule" */
                                    printf("Schedule\r\n");
                                    break;
                                
                                case CYBLE_ANCS_NS_CAT_ID_EML: /* If Category ID is "Email" */
                                    printf("Email\r\n");
                                    if(ns[nsCnt].evtId == CYBLE_ANCS_NS_EVT_ID_ADD) /* If Event ID is "Notification Added" */
                                    {
                                        if(nsCnt < (CYBLE_ANCS_NS_CNT - 1)) /* If notification queue is not full */
                                        {
                                            nsCnt++; /* Increment notification counter */
                                        }
                                    }
                                    break;
                                
                                case CYBLE_ANCS_NS_CAT_ID_NWS: /* If Category ID is "News" */
                                    printf("News\r\n");
                                    break;
                                
                                case CYBLE_ANCS_NS_CAT_ID_HNF: /* If Category ID is "Health and Fitness" */
                                    printf("Health and Fitness\r\n");
                                    break;
                                
                                case CYBLE_ANCS_NS_CAT_ID_BNF: /* If Category ID is "Business and Finance" */
                                    printf("Business and Finance\r\n");
                                    break;
                                
                                case CYBLE_ANCS_NS_CAT_ID_LOC: /* If Category ID is "Location" */
                                    printf("Location\r\n");
                                    break;
                                
                                case CYBLE_ANCS_NS_CAT_ID_ENT: /* If Category ID is "Entertainment" */
                                    printf("Entertainment\r\n");
                                    break;
                                
                                default:
                                    printf("Unsupported\r\n");
                                    break;
                            }
                        }
                    }
                    break;
            
                case CYBLE_ANCS_DS:
                    {
                        uint8 locLength = ((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->value->len;
                        uint8* locData = ((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->value->val;
                        
                        if((ancsFlag & CYBLE_ANCS_FLG_STR) != 0u) /* If there is unfinished string */
                        {
                            for(i = 0u; i < locLength; i++)
                            {
                                UART_DEB_UartPutChar(locData[i]); /* Print data */
                            }
                            
                            ds.currLength += (uint16)locLength; /* Update current length */
                            
                            if(ds.currLength == ds.length) /* If data is complete */
                            {
                                ancsFlag &= (uint8)~CYBLE_ANCS_FLG_STR; /* Finish the string */
                                AncsNextAct(); /* Perform next action */
                            }
                        }
                        else 
                        {
                            locLength -= 8u; /* Take into account only data length */
                            
                            ds.cmdId = (CYBLE_ANCS_CP_CMD_ID_T)locData[0u]; /* Unpack Command ID */
                            /* Unpack Notification UID */
                            ds.ntfUid = ((uint32)locData[1u]) +
                                        ((uint32)locData[2u] << 8u) +
                                        ((uint32)locData[3u] << 16u) +
                                        ((uint32)locData[4u] << 24u);
                            ds.attId = (CYBLE_ANCS_CP_ATT_ID_T)locData[5u]; /* Unpack Attribute ID */
                             
                            CyBle_Set16ByPtr((uint8*)&ds.length,
                            CyBle_Get16ByPtr(&locData[6u])); /* Unpack data length */
                            
                            ds.data = &locData[8u]; /* Extract data pointer */
                            
                            if((ds.cmdId == CYBLE_ANCS_CP_CMD_ID_GNA) && (memcmp(&ds.ntfUid, &cp.ntfUid, 4u) == 0))
                            { /* If Command ID is "Get Notification Attributes" and Notification UID matches */
                                switch(ds.attId)
                                {
                                    case CYBLE_ANCS_CP_ATT_ID_TTL: /* If Attribute ID is "Title" */
                                        switch(cp.ctgId)
                                        {
                                            case CYBLE_ANCS_NS_CAT_ID_EML: /* If Category ID is "Email" */
                                                printf("\r\nEmail from: \n");
                                                break;
                                                
                                            case CYBLE_ANCS_NS_CAT_ID_INC: /* If Category ID is "Incoming Call" */
                                                printf("\r\nIncoming Call from: ");
                                                break;
                                                
                                            case CYBLE_ANCS_NS_CAT_ID_SOC: /* If Category ID is "Social" */
                                                printf("\r\nApp: \n");
                                                break;
                                                
                                            default:
                                                printf("\r\nTitle: \n");
                                                break;
                                        }
                                        break;
                                        
                                    case CYBLE_ANCS_CP_ATT_ID_SBT: /* If Attribute ID is "Subtitle" */
                                        printf("Subject: \n");
                                        break;
                                        
                                    case CYBLE_ANCS_CP_ATT_ID_MSG: /* If Attribute ID is "Message" */
                                        printf("Message: \n");
                                        break;
                                        
                                    default:
                                        break;
                                }
                                
                                switch(ds.attId)
                                {
                                    case CYBLE_ANCS_CP_ATT_ID_TTL: /* If Attribute ID is "Title" */
                                    case CYBLE_ANCS_CP_ATT_ID_SBT: /* If Attribute ID is "Subtitle" */
                                    case CYBLE_ANCS_CP_ATT_ID_MSG: /* If Attribute ID is "Message" */
                                        for(i = 0u; i < locLength; i++)
                                        {
                                            UART_DEB_UartPutChar(ds.data[i]); /* Print data */
                                        }
                                        
                                        if(ds.length > locLength)
                                        { /* Indicate that data is not full, 
                                            waiting for next notification with rest of data */
                                            ancsFlag |= CYBLE_ANCS_FLG_STR; 
                                            ds.currLength = locLength; /* Initialise current length */
                                        }
                                        else
                                        { /* Otherwise perform next action */
                                            AncsNextAct();
                                        }
                                        break;
                                    
                                    default:
                                        break;
                                
                                }
                            }   
                        }
                    }
                    break;
                    
                default:
                    break;
            }
            break;
        
        case CYBLE_EVT_ANCSC_WRITE_CHAR_RESPONSE:
            ancsFlag &= (uint8)~CYBLE_ANCS_FLG_RSP;
            break;
                
        case CYBLE_EVT_ANCSC_WRITE_DESCR_RESPONSE:
            if(((CYBLE_ANCS_CHAR_VALUE_T *)eventParam)->charIndex == CYBLE_ANCS_NS) /* If NS notification is enabled */
            { /* Enable Data Source notification */
                cccd = CYBLE_CCCD_NOTIFICATION;
                apiResult = CyBle_AncscSetCharacteristicDescriptor(cyBle_connHandle, CYBLE_ANCS_DS, CYBLE_ANCS_CCCD, CYBLE_CCCD_LEN, (uint8*)&cccd);
            	if(apiResult != CYBLE_ERROR_OK)
            	{
            		printf("CyBle_AncscSetCharacteristicDescriptor API Error: ");
                    PrintApiResult();
            	}
            }
            break;
            
        case CYBLE_EVT_ANCSC_ERROR_RESPONSE:
            ancsFlag &= (uint8)~CYBLE_ANCS_FLG_RSP;
            break;
        
		default: /* Print unknown event number */
            printf("unknown ANCS event: 0x%lx\r\n", event);
			break;
    }
}


/*******************************************************************************
* Function Name: AncsAct
********************************************************************************
*
* Summary:
*   Initiates the ANCS action.
*
* Parameters:
*   actId - The ANCS action identifier.
*
* Return:
*   None.
*
*******************************************************************************/
void AncsAct(CYBLE_ANCS_CP_ACT_ID_T actId)
{
    cp.ntfUid = ns[nsCnt].ntfUid; /* Initialize control point Notification UID */
    cp.ctgId = ns[nsCnt].ctgId; /* Initialize control point Category ID */
    cp.cmdId = CYBLE_ANCS_CP_CMD_ID_PNA; /* Set control point Command ID to "Perform Notification Action" */
    cp.actId = actId; /* Initialize control point Action ID */
    ancsFlag |= CYBLE_ANCS_FLG_CMD; /* Trig control point command write */
    ancsFlag &= (uint8)~(CYBLE_ANCS_FLG_ACT | CYBLE_ANCS_FLG_NTF); /* Clear Action and Notification flags */
    Ringing_LED_Write(LED_OFF); /* Turn off ringing LED */
    if(actId == CYBLE_ANCS_CP_ACT_ID_POS)
    { /* If Positive action */
        printf("Accepted.\r\n");
    }
    else
    { /* Else Negative action */
        printf("Declined.\r\n");
    }
}


/*******************************************************************************
* Function Name: AncsProcess
********************************************************************************
*
* Summary:
*   Processes the Ansc functionality in the main loop.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void AncsProcess(void)
{
    if(CyBle_GattGetBusyStatus() != CYBLE_STACK_STATE_BUSY)
    {
        if((ancsFlag & CYBLE_ANCS_FLG_ACT) != 0u)
        {
            
            /* Button polling */
            if((flag & TIMER_SW2) != 0u)
            {
                flag &= (uint8)~TIMER_SW2;
               
                switch(button)
                {
                    case SW2_ONE_PRESSING:
                        if((ns[nsCnt].evtFlg & CYBLE_ANCS_NS_FLG_NA) != 0u)
                        {
                            AncsAct(CYBLE_ANCS_CP_ACT_ID_NEG); /* Trig negative action */
                        }
                        break;
                        
                    case SW2_TWO_PRESSING:
                        if((ns[nsCnt].evtFlg & CYBLE_ANCS_NS_FLG_PA) != 0u)
                        {
                            AncsAct(CYBLE_ANCS_CP_ACT_ID_POS); /* Trig positive action */
                        }
                        break;
                        
                    default:
                        break;
                }
                
                button = SW2_ZERO_PRESSING;
            }
        }
        else if((nsCnt > 0u) && ((ancsFlag & CYBLE_ANCS_FLG_NTF) == 0u))
        { /* If there are pending Notifications and current Notification is already processed */
            nsCnt--; /* Decrement Notification counter */
            cp.ntfUid = ns[nsCnt].ntfUid; /* Initialize control point Notification UID */
            cp.ctgId = ns[nsCnt].ctgId; /* Initialize control point Category ID */
            cp.cmdId = CYBLE_ANCS_CP_CMD_ID_GNA; /* Set control point Command ID to "Get Notification Attributes" */
            cp.attId = CYBLE_ANCS_CP_ATT_ID_TTL; /* Set control point Attribute ID to "Title" */
            ancsFlag |= CYBLE_ANCS_FLG_NTF | CYBLE_ANCS_FLG_CMD; /* Trig next Notification process */
        }
        else if((ancsFlag & CYBLE_ANCS_FLG_START) != 0u)
        { /* Start ANCS client working, this code performs only once after connecting toserver */
            ancsFlag &= (uint8)~CYBLE_ANCS_FLG_START;
            
            if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_ancsc.charInfo[CYBLE_ANCS_NS].descrHandle[CYBLE_ANCS_CCCD])
            { 
                if( CyBle_GetClientState() == CYBLE_CLIENT_STATE_CONNECTED ) 
                {
                    /* Discover server's database if it is not discovered */
                    StartDiscovery(); 
                }
            }
            else
            { /* Else enable Notification Source characteristic notification */
                CyBle_SetClientState(CYBLE_CLIENT_STATE_DISCOVERED);
                cccd = CYBLE_CCCD_NOTIFICATION;
                apiResult = CyBle_AncscSetCharacteristicDescriptor(cyBle_connHandle, CYBLE_ANCS_NS, CYBLE_ANCS_CCCD, CYBLE_CCCD_LEN, (uint8*)&cccd);
            	if(apiResult != CYBLE_ERROR_OK)
            	{
            		printf("CyBle_AncscSetCharacteristicDescriptor(NS) API Error: ");
                    PrintApiResult();
            	}
                else
                {
                    AncsPrintCharName(CYBLE_ANCS_NS);
                    printf("CCCD write request: 0x%2.2x\r\n", cccd);
                }
            }   
        }
        else
        {
        }
        
        if(((ancsFlag & CYBLE_ANCS_FLG_CMD) != 0u) && ((ancsFlag & CYBLE_ANCS_FLG_RSP) == 0u))
        { /* If there is pending command */
            uint8 attr[8u];
            uint8 length = 0u;
            
            ancsFlag &= (uint8)~CYBLE_ANCS_FLG_CMD;
            
            attr[0u] = cp.cmdId; /* Pack Command ID */
            for(i = 0u; i < 4u; i++)
            {
                attr[i + 1u] = (uint8)((cp.ntfUid >> (i << 3u)) & 0x000000FFu);
            } /* Pack Notification UID */
            
            switch(cp.cmdId)
            {
                case CYBLE_ANCS_CP_CMD_ID_GNA: /* If Command ID is "Get Notification Attributes" */
                    attr[5u] = (uint8)cp.attId; /* Pack Attribute ID */
                    CyBle_Set16ByPtr(&attr[6u], CYBLE_ANCS_MAX_STR_LENGTH); /* Pack maximum string length */
                    length = 8u; /* Length of this command */
                    break;
                
                case CYBLE_ANCS_CP_CMD_ID_PNA: /* If Command ID is "Perform Notification Action" */
                    attr[5u] = (uint8)cp.actId; /* Pack Action ID */
                    length = 6u; /* Length of this command */
                    break;
                
                default:
                    break;
            }
            
            if(length != 0u)
            { /* If there is packed command */
                (void) CyBle_AncscSetCharacteristicValue(cyBle_connHandle, CYBLE_ANCS_CP, length, attr);
                ancsFlag |= CYBLE_ANCS_FLG_RSP;
            }
        }
    }
}


/* [] END OF FILE */
