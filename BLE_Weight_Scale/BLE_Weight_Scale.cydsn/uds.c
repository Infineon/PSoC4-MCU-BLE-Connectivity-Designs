/*******************************************************************************
* File Name: uds.c
*
* Version: 1.0
*
* Description:
*  This file contains routines related to Weight Scale Service.
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
#include "uds.h"


/***************************************
*        Global Variables
***************************************/
uint8                      isUdsIndicationEnabled = NO;
uint8                      isUdsNotificationEnabled = NO;
uint8                      isUdsIndicationPending = NO;
uint8                      isUdsNotificationPending = NO;
UDS_USER_RECORD_T          udsUserRecordDef;
UDS_USER_RECORD_T          udsUserRecord[MAX_USERS];
uint8                      ucpResp[UDS_CP_RESPONSE_MAX_SIZE];
uint8                      udsIndDataSize;
uint8                      isUserRegistered[MAX_USERS];
uint8                      udsAccessDenied = YES;
uint8                      isUserHeightReceived = NO;
uint8                      isUserWeightReceived = NO;

/* Stores number of registered users. Maximum is 4. */
uint8                      udsRegisteredUserCount;


/*******************************************************************************
* Function Name: UdsCallBack
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component
*  specific to User Data Service.
*
* Parameters:
*  event - 		 A UDS event.
*  eventParams - The data structure specific to an event received.
*
*******************************************************************************/
void UdsCallBack(uint32 event, void *eventParam)
{
    uint8 i;
    CYBLE_UDS_CHAR_VALUE_T *udsCharValPtr;
    
    switch(event)
    {

    /****************************************************
    *              UDS Server events
    ****************************************************/
    /** UDS Server - Indication for User Data Service Characteristic
        was enabled. The parameter of this event is a structure 
        of the CYBLE_UDS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_UDSS_INDICATION_ENABLED:
        DBG_PRINTF("CYBLE_EVT_UDSS_INDICATION_ENABLED\r\n");
        isUdsIndicationEnabled = YES;
        break;

    /** UDS Server - Indication for User Data Service Characteristic
        was disabled. The parameter of this event is a structure 
        of the CYBLE_UDS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_UDSS_INDICATION_DISABLED:
        DBG_PRINTF("CYBLE_EVT_UDSS_INDICATION_DISABLED\r\n");
        isUdsIndicationEnabled = NO;
        break;

    /** UDS Server - User Data Service Characteristic
        Indication was confirmed. The parameter of this event
        is a structure of the CYBLE_UDS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_UDSS_INDICATION_CONFIRMED:
        DBG_PRINTF("CYBLE_EVT_WSSS_INDICATION_CONFIRMED\r\n");
        break;

    /** UDS Server - Notifications for User Data Service Characteristic
        were enabled. The parameter of this event is a structure of the
        CYBLE_UDS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_UDSS_NOTIFICATION_ENABLED:
        DBG_PRINTF("CYBLE_EVT_WSSS_INDICATION_CONFIRMED\r\n");
        isUdsNotificationEnabled = YES;
        break;

    /** UDS Server - Notifications for User Data Service Characteristic
        were disabled. The parameter of this event is a structure 
        of the CYBLE_UDS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_UDSS_NOTIFICATION_DISABLED:
        DBG_PRINTF("CYBLE_EVT_WSSS_INDICATION_CONFIRMED\r\n");
        isUdsNotificationEnabled = NO;
        break;

   /** UDS Server - Read Request for User Data Service Characteristic 
        was received. The parameter of this event is a structure
        of the CYBLE_UDS_CHAR_VALUE_T type.
    */    
    case CYBLE_EVT_UDSS_READ_CHAR:
        DBG_PRINTF("CYBLE_EVT_UDSS_READ_CHAR\r\n");

        udsCharValPtr = (CYBLE_UDS_CHAR_VALUE_T *) eventParam;

        /* If user didn't provide correct Consent for current user index,
        * then no characteristic can be read.
        */
        if((udsAccessDenied == YES) && (udsCharValPtr->charIndex != CYBLE_UDS_UIX) && 
            (udsCharValPtr->charIndex != CYBLE_UDS_DCI))
        {
            udsCharValPtr->gattErrorCode = CYBLE_GATT_ERR_USER_DATA_ACCESS_NOT_PERMITTED;
        }
        DBG_PRINTF("OK\r\n");
        break;
        
    /** UDS Server - Write Request for User Data Service Characteristic 
        was received. The parameter of this event is a structure
        of the CYBLE_UDS_CHAR_VALUE_T type.
    */    
    case CYBLE_EVT_UDSS_WRITE_CHAR:
        DBG_PRINTF("CYBLE_EVT_UDSS_WRITE_CHAR\r\n");

        udsCharValPtr = (CYBLE_UDS_CHAR_VALUE_T *) eventParam;
        DBG_PRINTF("Char index: %d\r\n", udsCharValPtr->charIndex);

        if(udsCharValPtr->charIndex != CYBLE_UDS_UCP)
        {
            if(userIndex != UDS_UNKNOWN_USER)
            {
                if((udsAccessDenied == YES) && (udsCharValPtr->charIndex != CYBLE_UDS_DCI))
                {
                    DBG_PRINTF("Characteristic value wasn't written: ");
                    DBG_PRINTF("Access denied. No consent provided.\n\r");
                    udsCharValPtr->gattErrorCode = CYBLE_GATT_ERR_USER_DATA_ACCESS_NOT_PERMITTED;
                }
                else
                {
                    DBG_PRINTF("Characteristic value: ");

                    for(i = 0u; i < udsCharValPtr->value->len; i++)
                    {
                        DBG_PRINTF("%2.2x", udsCharValPtr->value->val[i]);
                    }
                    DBG_PRINTF("\n\r");

                    UdsUpdateUserRecord(udsCharValPtr->charIndex, udsCharValPtr->value);
                    
                    /* Need to update Database Change Increment characteristic */
                    UdsUpdateDatabaseChangeIncrement();
                }
            }
            else
            {
                DBG_PRINTF("Characteristic value wasn't written: ");

                if(udsAccessDenied != YES)
                {
                    DBG_PRINTF("Access denied. No consent provided.\n\r");
                }
                else
                {
                    DBG_PRINTF("No user registered.\n\r");
                }
                udsCharValPtr->gattErrorCode = CYBLE_GATT_ERR_USER_DATA_ACCESS_NOT_PERMITTED;
            }
        }
        else
        {
            UdsHandleCpResponse(udsCharValPtr->value->val);
        }
        break;

    /****************************************************
    *               UDS Client events
    * These events are not active for the Server device
    * operation. They are added as a template for the
    * Client mode operation to ease the user experience
    * with the User Data Service.
    ****************************************************/
    
    /** UDS Client - User Data Service Characteristic
        Indication was received. The parameter of this event
        is a structure of CYBLE_UDS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_UDSC_INDICATION:
        
        break;

    /** UDS Client - User Data Service Characteristic
        Notification was received. The parameter of this event
        is a structure of CYBLE_UDS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_UDSC_NOTIFICATION:
        
        break;

    /** UDS Client - Read Response for Read Request of User Data 
        Service Characteristic value. The parameter of this event
        is a structure of CYBLE_UDS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_UDSC_READ_CHAR_RESPONSE:
        
        break;

    /** UDS Client - Write Response for Write Request of User Data 
        Service Characteristic value. The parameter of this event
        is a structure of CYBLE_UDS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_UDSC_WRITE_CHAR_RESPONSE:
        
        break;

    /** UDS Client - Read Response for Read Request of User Data
        Service Characteristic Descriptor Read request. The 
        parameter of this event is a structure of
        CYBLE_UDS_DESCR_VALUE_T type.
    */
    case CYBLE_EVT_UDSC_READ_DESCR_RESPONSE:
        
        break;

    /** UDS Client - Write Response for Write Request of User Data
        Service Characteristic Configuration Descriptor value.
        The parameter of this event is a structure of 
        CYBLE_UDS_DESCR_VALUE_T type.
    */
    case CYBLE_EVT_UDSC_WRITE_DESCR_RESPONSE:
        
        break;
    
    /** UDS Client - Error Response for Write Request for User Data Service
        Characteristic Value. The parameter of this event is a structure of 
        CYBLE_UDS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_UDSC_ERROR_RESPONSE:
        
        break;

	default:
        DBG_PRINTF("Unrecognized UDS event.\r\n");
	    break;
    }
}


/*******************************************************************************
* Function Name: UdsInit
********************************************************************************
*
* Summary:
*  Initializes the variables related to User Data Service's Characteristics 
*  with values from the BLE component customizer's GUI. 
*
*******************************************************************************/
void UdsInit(void)
{
    uint8 i;
    CYBLE_API_RESULT_T apiResult;
    uint8 rdData[UDS_LAST_NAME_LENGTH]; 
    
    /* Register event handler for UDS specific events */
    CyBle_UdsRegisterAttrCallback(UdsCallBack);
    
    /* The initial value of the User Index characteristic is ignored in
    * this example. The users' index starts from 0 and goes up to 3.
    */
    userIndex = UDS_DEFAULT_USER;
    
    isUserRegistered[UDS_DEFAULT_USER] = YES;

    udsUserRecordDef.consent = UDS_DEFAULT_CONSENT;
    
    /* Read initial value of First Name Characteristic */
    apiResult = CyBle_UdssGetCharacteristicValue(CYBLE_UDS_FNM, UDS_FIRST_NAME_LENGTH, rdData);

    if(apiResult == CYBLE_ERROR_OK)
    {
        DBG_PRINTF("First Name Characteristic was read successfully \r\n");

        memcpy(udsUserRecordDef.firstName, rdData, UDS_FIRST_NAME_LENGTH);
        
        for(i = UDS_DEFAULT_USER; i < MAX_USERS; i++)
        {
            memcpy(udsUserRecord[i].firstName, rdData, UDS_FIRST_NAME_LENGTH); 
        }
    }
    else
    {
        DBG_PRINTF("Error while reading First Name Characteristic. Error code: %d \r\n", apiResult);
    }

    /* Read initial value of Last Name Characteristic */
    apiResult = CyBle_UdssGetCharacteristicValue(CYBLE_UDS_LNM, UDS_LAST_NAME_LENGTH, rdData);

    if(apiResult == CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Last Name Characteristic was read successfully \r\n");

        memcpy(udsUserRecordDef.lastName, rdData, UDS_LAST_NAME_LENGTH);
        
        for(i = UDS_DEFAULT_USER; i < MAX_USERS; i++)
        {
            memcpy(udsUserRecord[i].lastName, rdData, UDS_LAST_NAME_LENGTH);  
        }
    }
    else
    {
        DBG_PRINTF("Error while reading Last Name Characteristic. Error code: %d \r\n", apiResult);
    }

    /* Read initial value of Age Characteristic */
    apiResult = CyBle_UdssGetCharacteristicValue(CYBLE_UDS_AGE, 1u, &udsUserRecordDef.age);

    if(apiResult == CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Age Characteristic was read successfully \r\n");
        
        /* Fill the Age Characteristic value for all users with value from
        * component customizer.
        */
        for(i = UDS_DEFAULT_USER; i < MAX_USERS; i++)
        {
            udsUserRecord[i].age = udsUserRecordDef.age;
        }
    }
    else
    {
        DBG_PRINTF("Error while reading Age Characteristic. Error code: %d \r\n", apiResult);
    }

    /* Read initial value of Gender Characteristic */
    apiResult = CyBle_UdssGetCharacteristicValue(CYBLE_UDS_GND, 1u, &udsUserRecordDef.gender);

    if(apiResult == CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Gender Characteristic was read successfully \r\n");
        
        /* Fill Gender Characteristic value for all users with value from
        * component customizer.
        */
        for(i = UDS_DEFAULT_USER; i < MAX_USERS; i++)
        {
            udsUserRecord[i].gender = udsUserRecordDef.gender;
        }
    }
    else
    {
        DBG_PRINTF("Error while reading Gender Characteristic. Error code: %d \r\n", apiResult);
    }

    /* Read initial value of Weight Characteristic */
    apiResult = CyBle_UdssGetCharacteristicValue(CYBLE_UDS_WGT, 2u, rdData);

    if(apiResult == CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Weight Characteristic was read successfully \r\n");
        
        udsUserRecordDef.weight = PACK_U16(rdData[0u], rdData[1u]);

        /* Fill Weight Characteristic value for all users with value from
        * component customizer.
        */
        for(i = UDS_DEFAULT_USER; i < MAX_USERS; i++)
        {
            udsUserRecord[i].weight = udsUserRecordDef.weight;
        }
    }
    else
    {
        DBG_PRINTF("Error while reading Weight Characteristic. Error code: %d \r\n", apiResult);
    }

    /* Read initial value of Height Characteristic */
    apiResult = CyBle_UdssGetCharacteristicValue(CYBLE_UDS_HGT, 2u, (uint8 *) &udsUserRecordDef.height);

    if(apiResult == CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Height Characteristic was read successfully \r\n");
        
        /* Fill Height Characteristic value for all users with value from
        * component customizer.
        */
        for(i = UDS_DEFAULT_USER; i < MAX_USERS; i++)
        {
            udsUserRecord[i].height = udsUserRecordDef.height;
        }
    }
    else
    {
        DBG_PRINTF("Error while reading Height Characteristic. Error code: %d \r\n", apiResult);
    }

    /* Read initial value of Database Change Increment Characteristic */
    apiResult =
        CyBle_UdssGetCharacteristicValue(CYBLE_UDS_DCI, 4u, (uint8 *) &udsUserRecordDef.dbChangeIncrement);

    if(apiResult == CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Database Change Increment Characteristic was read successfully \r\n");
        
        /* Fill Database Change Increment Characteristic value for all users with
        * value from the component customizer.
        */
        for(i = UDS_DEFAULT_USER; i < MAX_USERS; i++)
        {
            udsUserRecord[i].dbChangeIncrement = udsUserRecordDef.dbChangeIncrement;
        }
    }
    else
    {
        DBG_PRINTF("Error while reading Database Change Increment Characteristic. Error code: %d \r\n", apiResult);
    }
    
    udsRegisteredUserCount = 1u;
}


/*******************************************************************************
* Function Name: UdsHandleCpResponse
********************************************************************************
*
* Summary:
*  Handles sending the response for UDS User Control Point.
*
* Parameters:  
*  charValue: UDS User Control Point Characteristic value written by the Client.
*
*******************************************************************************/
void UdsHandleCpResponse(uint8 *charValue)
{
    uint8 i;
    uint8 byteCount = 0u;
    CYBLE_API_RESULT_T apiResult;

    ucpResp[byteCount++] = UDS_CP_RESPONSE;

    if(charValue != NULL)
    {
        switch (charValue[0u])
        {
        /* Handler for Register New User command */
        case UDS_CP_REGISTER_NEW_USER:
            if(udsRegisteredUserCount < MAX_USERS)
            {
                for(i = 0u; i < MAX_USERS; i++)
                {
                    if(isUserRegistered[i] == NO)
                    {
                        /* Increment user records count */
                        udsRegisteredUserCount++;
                        isUserRegistered[i] = YES;
                        userIndex = i;
                        break;
                    }
                }

                /* Clear access denied flag */
                udsAccessDenied = NO;

                /* Fill in received consent for new user */
                udsUserRecord[userIndex].consent =
                    (((uint16) charValue[UDS_CP_PARAM_BYTE2_IDX]) << 8u) | ((uint16) charValue[UDS_CP_PARAM_BYTE1_IDX]);
                        
                DBG_PRINTF("New user registered. User ID: %d. Consent: 0x%4.4x\r\n", userIndex,
                            udsUserRecord[udsRegisteredUserCount - 1u].consent);

                /* Form response packet */
                ucpResp[byteCount++] = UDS_CP_REGISTER_NEW_USER;
                ucpResp[byteCount++] = UDS_CP_RESP_VALUE_SUCCESS;
                ucpResp[byteCount++] = userIndex;

                /* Set active user's ID to User ID Characteristics */
                apiResult = CyBle_UdssSetCharacteristicValue(CYBLE_UDS_UIX, 1u, &userIndex);

                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_UdssSetCharacteristicValue() API Error: %x \r\n", apiResult);
                }
                else
                {
                    DBG_PRINTF("CyBle_UdssSetCharacteristicValue() Success \r\n");
                }
            }
            else
            {
                ucpResp[byteCount++] = charValue[0u];
                ucpResp[byteCount++] = UDS_CP_RESP_OPERATION_FAILED;
                DBG_PRINTF("Too many user records\r\n");
            }
            break;
        case UDS_CP_CONSENT:
            ucpResp[byteCount++] = UDS_CP_CONSENT;
            if(charValue[UDS_CP_PARAM_BYTE1_IDX] < MAX_USERS)
            {
                if(isUserRegistered[charValue[UDS_CP_PARAM_BYTE1_IDX]] == YES)
                {
                    if(udsUserRecord[charValue[UDS_CP_PARAM_BYTE1_IDX]].consent ==
                        ((uint16)((((uint16) charValue[UDS_CP_PARAM_BYTE3_IDX]) << 8u) |
                            ((uint16) charValue[UDS_CP_PARAM_BYTE2_IDX]))))
                    {
                        ucpResp[byteCount++] = UDS_CP_RESP_VALUE_SUCCESS;

                        /* Clear access denied flag */
                        udsAccessDenied = NO;

                        DBG_PRINTF("Access allowed for: %s, %s (Index - %d).\r\n", 
                                udsUserRecord[userIndex].firstName,
                                udsUserRecord[userIndex].lastName,
                                userIndex);
                        UdsLoadUserDataToDb(charValue[UDS_CP_PARAM_BYTE1_IDX]);
                    }
                    else
                    {
                        ucpResp[byteCount++] = UDS_CP_RESP_USER_NOT_AUTHORIZED;
                        DBG_PRINTF("Wrong consent\r\n");
                    }
                }
                else
                {
                    DBG_PRINTF("User with index - %d is not registered.\r\n", charValue[UDS_CP_PARAM_BYTE1_IDX]);
                    ucpResp[byteCount++] = UDS_CP_RESP_OPERATION_FAILED;
                }
            }
            else
            {
                DBG_PRINTF("Invalid user index - %d.\r\n", charValue[UDS_CP_PARAM_BYTE1_IDX]);
                ucpResp[byteCount++] = UDS_CP_RESP_INVALID_PARAMETER;
            }
            break;
        case UDS_CP_DELETE_USER_DATA:
            if(udsRegisteredUserCount == 0u)
            {
                ucpResp[byteCount++] = UDS_CP_DELETE_USER_DATA;
                ucpResp[byteCount++] = UDS_CP_RESP_OPERATION_FAILED;
                DBG_PRINTF("Failed to delete user. No registered users\r\n");
            }
            else
            {
                ucpResp[byteCount++] = UDS_CP_DELETE_USER_DATA;
                ucpResp[byteCount++] = UDS_CP_RESP_VALUE_SUCCESS;

                DBG_PRINTF("User record for: %s, %s (Index - %d) is deleted.\r\n", 
                        udsUserRecord[userIndex].firstName,
                        udsUserRecord[userIndex].lastName,
                        userIndex);

                isUserRegistered[userIndex] = NO;

                /* Decrement user records count */
                udsRegisteredUserCount--;
                userIndex = UdsFindRegisteredUserIndex();

                if(userIndex != UDS_UNKNOWN_USER)
                {
                    /* Load data for the user whith userIndex-1 */
                    UdsLoadUserDataToDb(userIndex);
                }

                udsAccessDenied = YES;
            }
            break;
        default:
            ucpResp[byteCount++] = charValue[0u];
            ucpResp[byteCount++] = UDS_CP_RESP_OP_CODE_NOT_SUPPORTED;
            DBG_PRINTF("Unknown UCP Op Code\r\n");
            break;
        }

        udsIndDataSize = byteCount;

        /* Set flag that response is ready to be indicated */
        isUdsIndicationPending = YES;
    }
}


/*******************************************************************************
* Function Name: UdsLoadUserDataToDb
********************************************************************************
*
* Summary:
*  Loads all the data related to the user identified by "uIdx" into the GATT
*  database.
*
* Parameters:  
*  uIdx - User index.
*
*******************************************************************************/
void UdsLoadUserDataToDb(uint8 uIdx)
{
    uint8 buff[4u];
    CYBLE_API_RESULT_T apiResult;

    /* Set all User's characteristic into GATT DB */
    apiResult = CyBle_UdssSetCharacteristicValue(CYBLE_UDS_UIX, 1u, &uIdx);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Set User index - Error (Error Code: %x)\r\n", apiResult);
    }
    
    apiResult = CyBle_UdssSetCharacteristicValue(CYBLE_UDS_AGE, 1u, &udsUserRecord[uIdx].age);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Set Age - Error (Error Code: %x)\r\n", apiResult);
    }
    
    apiResult = CyBle_UdssSetCharacteristicValue(CYBLE_UDS_GND, 1u, &udsUserRecord[uIdx].gender);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Set Gender - Error (Error Code: %x)\r\n", apiResult);
    }
    
    apiResult =
        CyBle_UdssSetCharacteristicValue(CYBLE_UDS_FNM, UDS_FIRST_NAME_LENGTH, udsUserRecord[uIdx].firstName);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Set First Name - Error (Error Code: %x)\r\n", apiResult);
    }
    
    apiResult = CyBle_UdssSetCharacteristicValue(CYBLE_UDS_LNM, UDS_LAST_NAME_LENGTH, udsUserRecord[uIdx].lastName);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Set Last Name - Error (Error Code: %x)\r\n", apiResult);
    }
    
    buff[0u] = LO8(udsUserRecord[uIdx].height);
    buff[1u] = HI8(udsUserRecord[uIdx].height);
    apiResult = CyBle_UdssSetCharacteristicValue(CYBLE_UDS_HGT, 2u, buff);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Set Height - Error (Error Code: %x)\r\n", apiResult);
    }

    buff[0u] = LO8(udsUserRecord[uIdx].weight);
    buff[1u] = HI8(udsUserRecord[uIdx].weight);
    apiResult = CyBle_UdssSetCharacteristicValue(CYBLE_UDS_WGT, 2u, buff);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Set Weight - Error (Error Code: %x)\r\n", apiResult);
    }

    buff[0u] = LO8(LO16(udsUserRecord[uIdx].dbChangeIncrement));
    buff[1u] = HI8(LO16(udsUserRecord[uIdx].dbChangeIncrement));
    buff[2u] = LO8(HI16(udsUserRecord[uIdx].dbChangeIncrement));
    buff[3u] = HI8(HI16(udsUserRecord[uIdx].dbChangeIncrement));

    apiResult = CyBle_UdssSetCharacteristicValue(CYBLE_UDS_DCI, 4u, buff);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Set Database Change Increment - Error (Error Code: %x)\r\n", apiResult);
    }
}


/*******************************************************************************
* Function Name: UdsUpdateUserRecord
********************************************************************************
*
* Summary:
*  Performs an update of the Characteristic value identified by the "charIndex"
*  in the user records array. The "userIndex" contains the index of an active
*  user record set.
* 
*  The data is stored in the same byte order as in the GATT database.
*
* Parameters:  
*  charIndex - The characteristic index.
*  charValue - The characteristic value to be stored.
*
*******************************************************************************/
void UdsUpdateUserRecord(CYBLE_UDS_CHAR_INDEX_T charIndex, CYBLE_GATT_VALUE_T *charValue)
{
    if(charValue != NULL)
    {
        switch (charIndex)
        {
        case CYBLE_UDS_FNM:
            memcpy(udsUserRecord[userIndex].firstName, charValue->val, charValue->len); 
            break;
        case CYBLE_UDS_LNM:
            memcpy(udsUserRecord[userIndex].lastName, charValue->val, charValue->len);
            break;
        case CYBLE_UDS_AGE:
            udsUserRecord[userIndex].age = charValue->val[0u];
            break;
        case CYBLE_UDS_GND:
            udsUserRecord[userIndex].gender = charValue->val[0u];
            break;
        case CYBLE_UDS_WGT:
            memcpy((void *) &udsUserRecord[userIndex].weight, charValue->val, charValue->len);
            isUserWeightReceived = YES;
            break;
        case CYBLE_UDS_HGT:
            memcpy((void *) &udsUserRecord[userIndex].height, charValue->val, charValue->len);
            isUserHeightReceived = YES;
            break;
        case CYBLE_UDS_DCI:
            memcpy((void *) &udsUserRecord[userIndex].dbChangeIncrement, charValue->val, charValue->len);
            break;
        default:
            break;
        }
    }
}


/*******************************************************************************
* Function Name: UdsSetWeight
********************************************************************************
*
* Summary:
*  Sets weight for the currently active user both in GATT database and user
*  records array. 
*
* Parameters:  
*  weight - The weight to be set.
*
*******************************************************************************/
void UdsSetWeight(uint16 weight)
{
    uint8 buff[2u];
    CYBLE_API_RESULT_T apiResult;

    udsUserRecord[userIndex].weight = weight;

    buff[0u] = LO8(weight);
    buff[1u] = HI8(weight);

    apiResult = CyBle_UdssSetCharacteristicValue(CYBLE_UDS_WGT, 2u, buff);

    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Set Weight - Error (Error Code: %x)\r\n", apiResult);
    }

    /* Need to update Database Change Increment Characteristic */
    UdsUpdateDatabaseChangeIncrement();
}


/*******************************************************************************
* Function Name: UdsUpdateUserRecord
********************************************************************************
*
* Summary:
*  Sets height for the currently active user both in GATT database and user
*  records array. 
*
* Parameters:  
*  height - The height to be set.
*
*******************************************************************************/
void UdsSetHeight(uint16 height)
{
    uint8 buff[2u];
    CYBLE_API_RESULT_T apiResult;

    udsUserRecord[userIndex].height = height;

    buff[0u] = LO8(height);
    buff[1u] = HI8(height);

    apiResult = CyBle_UdssSetCharacteristicValue(CYBLE_UDS_WGT, 2u, (uint8 *) &buff);

    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Set Height - Error (Error Code: %x)\r\n", apiResult);
    }

    /* Need to update Database Change Increment Characteristic */
    UdsUpdateDatabaseChangeIncrement();
}


/*******************************************************************************
* Function Name: UdsUpdateDatabaseChangeIncrement
********************************************************************************
*
* Summary:
*  Updates the Database Change increment Characteristic for the active user both
*  in GATT database and user records array.
*
*******************************************************************************/
void UdsUpdateDatabaseChangeIncrement(void)
{
    CYBLE_API_RESULT_T apiResult;
    uint8 buff[4u];

    udsUserRecord[userIndex].dbChangeIncrement += 1u;

    buff[0u] = LO8(LO16(udsUserRecord[userIndex].dbChangeIncrement));
    buff[1u] = HI8(LO16(udsUserRecord[userIndex].dbChangeIncrement));
    buff[2u] = LO8(HI16(udsUserRecord[userIndex].dbChangeIncrement));
    buff[3u] = HI8(HI16(udsUserRecord[userIndex].dbChangeIncrement));

    apiResult = CyBle_UdssSetCharacteristicValue(CYBLE_UDS_DCI, 4u, buff);

    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Set Database Change Increment - Error (Error Code: %x)\r\n", apiResult);
    }

    isUdsNotificationPending = YES;
}


/*******************************************************************************
* Function Name: UdsFindRegisteredUserIndex
********************************************************************************
*
* Summary:
*  Returns the index of registered user. The search starts from the (MAX_USERS -
*  1u), where MAX_USERS = 4, and goes down to 0. If no registered users were
*  found the value of 0xFF will be returned.
*
* Return:
*  UDS_UNKNOWN_USER - No registered users found, 
*  other            - index of registered user.
*
*******************************************************************************/
uint8 UdsFindRegisteredUserIndex(void)
{
    uint8 i;

    for(i = (MAX_USERS - 1u); i > 0u; i--)
    {
        if(isUserRegistered[i] == YES)
        {
            break;
        }
    }

    if(i == 0u)
    {
        /* If there are no registered users, set user index to "unknown user" */
        i = UDS_UNKNOWN_USER;
    }

    return(i);
}


/* [] END OF FILE */
