/*******************************************************************************
* File Name: ota_mandatory.c
*
* Version: 1.20
*
* Description:
*  Provides an API that implement core functionality of OTA.
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
#include "cytypes.h"
#include "ota_mandatory.h"


#if CYDEV_FLASH_SIZE != 0x00040000u
 #error "This design is specifically targeted to parts with 256k of flash. Please change project device to BLE\
 silicon that has 256K Flash array. For example CY8C4248LQI-BL483."
#endif

#if defined(__ARMCC_VERSION)
    
#include <cytypes.h>

extern void Bootloader___main(void);
void InitializeBootloaderSRAM(void);
extern int main(void);
extern int $Super$$main(void);
extern void initialize_psoc(void);
#endif /*__ARMCC_VERSION*/


#if defined(__ARMCC_VERSION)
    /*******************************************************************************
    * Following code implements re-initialization of separate RAM segment containing
    * data that is required for BLE Stack operation.
    * This code is used in case of MDK compiler.
    *******************************************************************************/
    __attribute__ ((section(".bootloaderruntype"), zero_init))
    volatile uint32 CyReturnToBootloaddableAddress;
    __attribute__ ((section(".bootloaderruntype"), zero_init))
    extern volatile uint8 appType; 
    extern void Bootloader__main(void);
    void InitializeBootloaderSRAM(void);
    extern int main(void);
    extern int $Super$$main(void);
    extern void initialize_psoc(void);

    /*******************************************************************************
    * Function Name: _platform_post_lib_init
    ********************************************************************************
    *
    * Summary:
    *   This function is called by MDK compiler startup code.
    *
    * Parameters:
    *   None
    *
    *******************************************************************************/
    void _platform_post_lib_init()
    {
        initialize_psoc();
        InitializeBootloaderSRAM();
    }
#elif defined (__GNUC__)
    /*******************************************************************************
    * Following code implements re-initialization of separate RAM segment containing
    * data that is required for BLE Stack operation.
    * This code is used in case of GCC compiler
    *******************************************************************************/
    extern void Bootloader___libc_init_array(void);
    typedef unsigned char __cy_byte_align8 __attribute ((aligned (8)));
    struct __cy_region
    {
        __cy_byte_align8 *init; /* Initial contents of this region.  */
        __cy_byte_align8 *data; /* Start address of region.  */
        size_t init_size;       /* Size of initial data.  */
        size_t zero_size;       /* Additional size to be zeroed.  */
    };
    extern const struct __cy_region Bootloader___cy_regions[];
    extern const char Bootloader___cy_region_num __attribute__((weak));
#define Bootloader___cy_region_num ((size_t)&Bootloader___cy_region_num)
    

    /*******************************************************************************
    * Function Name: Bootloader_Start_c
    ********************************************************************************
    *
    * Summary:
    *   This function initializes bootloader RAM .bss section. It is required for 
    *   BLE Stack operation.
    *
    * Parameters:
    *   None
    *
    *******************************************************************************/
    void Bootloader_Start_c(void)
    {
    unsigned regions = Bootloader___cy_region_num;
    const struct __cy_region *rptr = Bootloader___cy_regions;

        /* Initialize memory */
        for (regions = Bootloader___cy_region_num; regions != 0u; regions--)
        {
            uint32 *src = (uint32 *)rptr->init;
            uint32 *dst = (uint32 *)rptr->data;
            unsigned limit = rptr->init_size;
            unsigned count;

            for (count = 0u; count != limit; count += sizeof (uint32))
            {
                *dst = *src;
                dst++;
                src++;
            }
            limit = rptr->zero_size;
            for (count = 0u; count != limit; count += sizeof (uint32))
            {
                *dst = 0u;
                dst++;
            }

            rptr++;
        }

        /* Invoke static objects constructors using Bootloader__libc_init_array() function call here
         * if they are needed.
         */
}   
#elif defined (__ICCARM__)
    /*******************************************************************************
    * Following code implements re-initialization of separate RAM segment containing
    * data that is required for BLE Stack operation.
    * This code is used in case of IAR compiler.
    *******************************************************************************/
    extern void Bootloader___iar_data_init3(void);
#endif  /* (__ARMCC_VERSION) */


/*******************************************************************************
* Function Name: Bootloader_Start_c
********************************************************************************
*
* Summary:
*   This function triggers initialization of bootloader RAM .bss section.
*   It is required for BLE Stack operation. This function should be called from
*   main() prior BLE component start.
*
* Parameters:
*   None
*
*******************************************************************************/
void InitializeBootloaderSRAM()
{
#if defined(__ARMCC_VERSION)
     CyReturnToBootloaddableAddress = (uint32)$Super$$main;
     Bootloader___main();
#elif defined (__GNUC__)
    Bootloader_Start_c();
#elif defined (__ICCARM__)
    Bootloader___iar_data_init3();
#endif /* defined(__ARMCC_VERSION) */
}


/*******************************************************************************
* Function Name: Bootloadable_WriteFlashByte
********************************************************************************
*
* Summary:
*   This API writes to flash the specified data.
*
* Parameters:
*    address    - The address in flash.
*    inputValue - One-byte data.
*
* Return:
*   A status of the writing to flash procedure.
*
*******************************************************************************/
cystatus Bootloadable_WriteFlashByte(const uint32 address, const uint8 inputValue)
{
    cystatus result = CYRET_SUCCESS;
    uint32 flsAddr = address - CYDEV_FLASH_BASE;
    uint8  rowData[CYDEV_FLS_ROW_SIZE];

    uint16 rowNum = ( uint16 )(flsAddr / CYDEV_FLS_ROW_SIZE);

    uint32 baseAddr = address - (address % CYDEV_FLS_ROW_SIZE);
    uint16 idx;

    for(idx = 0u; idx < CYDEV_FLS_ROW_SIZE; idx++)
    {
        rowData[idx] = (uint8)Bootloadable_GET_CODE_DATA(baseAddr + idx);
    }

    rowData[address % CYDEV_FLS_ROW_SIZE] = inputValue;

    result = CySysFlashWriteRow((uint32) rowNum, rowData);

    return (result);
}


/*******************************************************************************
* Function Name: Bootloadable_SetActiveApplication
****************************************************************************//**
*
* Summary:
*   Sets the application which will be loaded after a next reset event.          
*   This API sets in the Flash (metadata section) the given active application 
*   number.
*          
*   NOTE The active application number is not set directly, but the boolean 
*   mark instead means that the application is active or not for the relative 
*   metadata. Both metadata sections are updated. For example, if the second 
*   application is to be set active, then in the metadata section for the first 
*   application there will be a "0" written, which means that it is not active, and 
*   for the second metadata section there will be a "1" written, which means that it is 
*   active. 
*
*   NOTE Intended for the combination project type ONLY!
*
* Parameters:
*   appId:
*       The active application number to be written to flash (metadata section) 
*       NOTE Possible values are:
*       0 - for the first application
*       1 - for the second application.
*       Any other number is considered invalid. 
*
* Return:
*   A status of writing to flash operation.
*       CYRET_SUCCESS - Returned if appId was successfully changed. 
*       CYRET_BAD_PARAM - Returned if the parameter appID passed to the
*                function has the same value as the active application ID. 
*
*******************************************************************************/
cystatus Bootloadable_SetActiveApplication(uint8 appId)
{
    cystatus result = CYRET_SUCCESS;

    uint8 CYDATA idx;
    
    /* If invalid application number */
    if (appId > Bootloadable_MD_BTLDB_ACTIVE_1)
    {
        result = CYRET_BAD_PARAM;
    }
    else
    {
        /* If appID has the same value as active application ID */
        if (1u == Bootloadable_GET_CODE_DATA(Bootloadable_MD_BTLDB_ACTIVE_OFFSET(appId)))
        {
            result = CYRET_BAD_PARAM;
        }
        else
        {
            /* Updating metadata section */
            for(idx = 0u; idx < Bootloadable_MAX_NUM_OF_BTLDB; idx++)
            {
                result |= Bootloadable_WriteFlashByte((uint32) Bootloadable_MD_BTLDB_ACTIVE_OFFSET(idx), \
                                                                                            (uint8)(idx == appId));
            }
        }
    }
    
    return (result);
}



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
void AfterImageUpdate(void)
{
    #if ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
        CYBLE_GAP_BONDED_DEV_ADDR_LIST_T bondedDevList;
    
        #if (PRINT_BOUNDING_DATA == YES)
            uint32 i;
            uint32 j;
        #endif
    #endif /* ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)) */
        
    if (0u == (uint32) CY_GET_XTND_REG8((volatile uint8 *)UPDATE_FLAG_OFFSET))
    {
        DBG_PRINT_TEXT("Application project was updated. ");
        
        #if ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
            if (CYBLE_GATT_DB_CCCD_COUNT == (uint32) CY_GET_XTND_REG8((volatile uint8 *)STACK_UPDATE_FLAG_OFFSET))
            {
                DBG_PRINT_TEXT("CCCD number has not changed.\r\n");
            }
            else
            {
                DBG_PRINT_TEXT("CCCD number has changed.\r\n");
                DBG_PRINT_TEXT("Erasing bounding data...");
                
                CyBle_GapGetBondedDevicesList(&bondedDevList);
                
                /* Clean bounded device list. */
                Clear_ROM_Array((uint8 *)&cyBle_flashStorage, sizeof(cyBle_flashStorage));
                
                Bootloadable_SetFlashByte(STACK_UPDATE_FLAG_OFFSET, CYBLE_GATT_DB_CCCD_COUNT);
            }
        #endif /* ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)) */
        
        Bootloadable_SetFlashByte(UPDATE_FLAG_OFFSET, 1u);
        
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

/* [] END OF FILE */
