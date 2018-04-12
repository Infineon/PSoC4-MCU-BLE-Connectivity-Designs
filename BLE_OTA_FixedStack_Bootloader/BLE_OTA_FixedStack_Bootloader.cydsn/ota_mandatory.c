/*******************************************************************************
* File Name: ota_mandatory.c
*
* Version: 1.40
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
#if defined(__ARMCC_VERSION)
    #include <cytypes.h>
    #include <Bootloader_PVT.h>
#endif /* defined(__ARMCC_VERSION) */


/* Buffer for received data */
uint8 packetRX[BLE_PACKET_SIZE_MAX];
uint32 packetRXSize;
uint32 packetRXFlag;

uint8 packetTX[BLE_PACKET_SIZE_MAX];
uint32 packetTXSize;

#if defined(__ARMCC_VERSION)
    
/* Provide APIs to allow bootloadable to initialize SRAM for BLE without getting stuck in bootloader */
__attribute__ ((section(".bootloaderruntype"), zero_init))
volatile uint32 CyReturnToBootloaddableAddress;
static void CyReturnToBootloaddable(uint32 appAddr);
    
__asm static void CyReturnToBootloaddable(uint32 appAddr)
{
    BX  R0
    ALIGN
}

void _platform_post_lib_init()
{
    uint32 addr;
    /***************************************************************************
    * Set CyReturnToBootloaddableAddress to zero in the case of a non-software
    * reset. This means that when the Bootloader application is scheduled -
    * the initial state is clean. The value of CyReturnToBootloaddableAddress
    * is valid only in the case of a software reset.
    * NOTE: This code is PSoC4 only, because BLE component is PSoC4 only.
    ***************************************************************************/
    if (0u == (Bootloader_RES_CAUSE_REG & Bootloader_RES_CAUSE_RESET_SOFT))
    {
        CyReturnToBootloaddableAddress = 0u;
    }
    
    if (0u != CyReturnToBootloaddableAddress)
    {
        addr = CyReturnToBootloaddableAddress;
        CyReturnToBootloaddableAddress = 0u;
        CyReturnToBootloaddable(addr);
    }
}


#endif /* __ARMCC_VERSION */

/* [] END OF FILE */
