/*******************************************************************************
* File Name: ota_optional.c
*
* Version: 1.20
*
* Description:
*  Provides an API that implement optional functionality of OTA.
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
#include "ota_optional.h"



/*******************************************************************************
* Function Name: AfterImageUpdate
********************************************************************************
*
* Summary:
*   This function checks if Self Project Image has been Updated and is running
*   for the first time. If so, and if Bonding data is used then it verifies
*   bonding data and erases bonding data if it is not valid
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void AfterImageUpdate()
{
    #if ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
        #if (PRINT_BOUNDING_DATA == YES)
            uint32 i;
            uint32 j;
        #endif
    #endif /* ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)) */
    
    if (0u == (uint32) CY_GET_XTND_REG8((volatile uint8 *)UPDATE_FLAG_OFFSET))
    {
        DBG_PRINT_TEXT("Stack project was updated.");
        
        #if ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
            /* Clean bounded device list. */
            Clear_ROM_Array((uint8 *)&cyBle_flashStorage, sizeof(cyBle_flashStorage));
        #endif /* ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)) */
            
        /* Set byte in metadata row indicating that Stack project was started. */
        Loader_SetFlashByte(UPDATE_FLAG_OFFSET, 1u);
        
        DBG_PRINT_TEXT("\r\n");
        DBG_PRINT_TEXT("\r\n");
    }
    
    #if ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
        #if (PRINT_BOUNDING_DATA == YES)
            /* Print bounding data if it was enabled in options.h */
            if (CYBLE_GATT_DB_CCCD_COUNT > 0u)
            {
                DBG_PRINT_TEXT("CCCD array(s):\r\n");
                for (i = 0u; i <= CYBLE_GAP_MAX_BONDED_DEVICE; i++)
                {
                    DBG_PRINTF("%lu: ", i);
                    for (j = 0u; j < CYBLE_GATT_DB_CCCD_COUNT; j++)
                    {
                        DBG_PRINTF("0x%02x ", cyBle_flashStorage.attValuesCCCDFlashMemory[i][j]);
                    }
                    DBG_PRINT_TEXT("\r\n");
                }
            }
            
            DBG_PRINT_TEXT("\r\nBounding array:\r\n");
            j = 0u;
            for (i = 0u; i < sizeof(cyBle_flashStorage.stackFlashptr); i++)
            {
                if (j < LENGHT_OF_UART_ROW)
                {
                    DBG_PRINTF("0x%02x ", cyBle_flashStorage.stackFlashptr[i]);
                    j++;
                }
                else
                {
                    DBG_PRINTF("0x%02x\r\n", cyBle_flashStorage.stackFlashptr[i]);
                    j = 0u;
                }
            }
            DBG_PRINT_TEXT("\r\n");
            DBG_PRINT_TEXT("\r\n");
        #endif /* (PRINT_BOUNDING_DATA == YES) */
    #endif /* ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)) */
}


#if ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
/*******************************************************************************
* Function Name: Clear_ROM_Array
********************************************************************************
*
* Summary:
*   Clears specified area in ROM.
*
* Parameters:
*   const uint8 eepromPtr[]:
*        Pointer to ROM to be cleared
*   uint32 byteCount:
*        Size of area to be cleared in bytes
*
* Return:
*   CYRET_UNKNOWN - On failure operation.
*   CYRET_SUCCESS - Operation successfully completed.
*
*******************************************************************************/
cystatus Clear_ROM_Array(const uint8 eepromPtr[], uint32 byteCount)
{
    uint8 writeBuffer[CY_FLASH_SIZEOF_ROW];
    uint32 rowId;
    uint32 dstIndex;
    uint32 srcIndex;
    cystatus rc;
    uint32 dataOffset;
    uint32 byteOffset;
    
    dataOffset = (uint32)eepromPtr;
    
    if (((uint32)eepromPtr + byteCount) < (CYDEV_FLASH_BASE+CYDEV_FLASH_SIZE))
    {
        rowId = (dataOffset / CY_FLASH_SIZEOF_ROW);
        byteOffset = (CY_FLASH_SIZEOF_ROW * rowId);
        srcIndex = 0u;

        rc = CYRET_SUCCESS;

        while ((srcIndex < byteCount) && (CYRET_SUCCESS == rc))
        {
            /* Fill only needed data with zeros. */
            for (dstIndex = 0u; dstIndex < CY_FLASH_SIZEOF_ROW; dstIndex++)
            {
                if ((byteOffset >= dataOffset) && (srcIndex < byteCount))
                {
                    writeBuffer[dstIndex] = 0x00;
                    srcIndex++;
                }
                else
                {
                    writeBuffer[dstIndex] = CY_GET_XTND_REG8(CYDEV_FLASH_BASE + byteOffset);
                }
                byteOffset++;
            }

            rc = CySysFlashWriteRow(rowId, writeBuffer);
            
            /* Go to the next row */
            rowId++;
        }
    }
    else
    {
        rc = CYRET_BAD_PARAM;
    }
    
    /* Mask return codes from flash, if they are not supported */
    if ((CYRET_SUCCESS != rc) && (CYRET_BAD_PARAM != rc))
    {
        rc = CYRET_UNKNOWN;
    }
    
    return (rc);
}
#endif /* ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)) */


void TimeoutImplementation()
{
    static uint16 counter = 1u;
    
    if (counter > 0u)
    {
        if ((Loader_isBootloading != Loader_BOOTLOADING_IN_PROGRESS) && (CyBle_GetState() != CYBLE_STATE_CONNECTED))
        {
            counter++;
        }
    }
    
    if (counter == WARNING_TIMEOUT)
    {
        if (CYRET_SUCCESS != Loader_ValidateApp(1u))
        {
            /* If bootloadable image is invalid - wait for the valid image to be received. */
            DBG_PRINT_TEXT("BLE Upgradable Stack Application image is invalid. Will stay here...\r\n");
            counter = 0u;
        }
        else
        {
            DBG_PRINT_TEXT("Inactivity will cause switch to the application...\r\n");   
        }
    }
    else if (counter >= SWITCHING_TIMEOUT)
    {
        /* Automatical switch to the Application project will occur only if Application image is valid
         * after ~300 seconds of inactivity.
         */
        DBG_PRINT_TEXT("Switching to BLE Upgradable Stack Application...\r\n");      
        CyDelay(500u);
        Bootloadable_SetActiveApplication(1u);
        CySoftwareReset();
    }
    
    if(SW2_S_Read() == 0u)
    {
        CyDelay(500);
        if(SW2_S_Read() == 0u)
        {
            #if (DEBUG_UART_ENABLED == YES)
                DBG_PRINT_TEXT("Switching to BLE Upgradable Stack Application...\r\n");
                CyDelay(500);
            #endif /* (DEBUG_UART_ENABLED == YES) */
                if(SW2_S_Read() == 0u)
                {
                    DBG_PRINT_TEXT("Release SW2 button to proceed...\r\n");
                    while (SW2_S_Read() == 0u)
                    {
                        /* Wait for button to be released */
                    }
                }

            if (CYRET_SUCCESS == Loader_ValidateApp(1u))
            {
                Bootloadable_SetActiveApplication(1u);
                CySoftwareReset();
            }
            else
            {
                DBG_PRINT_TEXT("BLE Upgradable Stack Application is not valid.\r\n");
            }
        }
    }
}

/* [] END OF FILE */
