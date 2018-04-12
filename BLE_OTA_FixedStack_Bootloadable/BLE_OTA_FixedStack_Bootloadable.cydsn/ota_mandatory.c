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
#include "common.h"

#if defined(__ARMCC_VERSION)
    
#include <cytypes.h>

__attribute__ ((section(".bootloaderruntype"), zero_init))
extern volatile uint8 appType; 
__attribute__ ((section(".bootloaderruntype"), zero_init))
volatile uint32 CyReturnToBootloaddableAddress;
extern void Bootloader__main(void);
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
    extern void Bootloader__main(void);
    void InitializeBootloaderSRAM(void);
    extern int main(void);
    extern int $Super$$main(void);
    extern void initialize_psoc(void);

    /*******************************************************************************
    * Function Name: _platform_post_lib_init()
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
    extern void Bootloader__libc_init_array(void);
    typedef unsigned char __cy_byte_align8 __attribute ((aligned (8)));
    struct __cy_region
    {
        __cy_byte_align8 *init; /* Initial contents of this region.  */
        __cy_byte_align8 *data; /* Start address of region.  */
        size_t init_size;       /* Size of initial data.  */
        size_t zero_size;       /* Additional size to be zeroed.  */
    };
    extern const struct __cy_region Bootloader__cy_regions[];
    extern const char Bootloader__cy_region_num __attribute__((weak));
    
    
    #define Bootloader__cy_region_num ((size_t)&Bootloader__cy_region_num)
    

    /*******************************************************************************
    * Function Name: Bootloader_Start_c()
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
        unsigned regions = Bootloader__cy_region_num;
        const struct __cy_region *rptr = Bootloader__cy_regions;

        /* Initialize memory */
        for (regions = Bootloader__cy_region_num; regions != 0u; regions--)
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
    extern void Bootloader__iar_data_init3(void);
#endif  /* (__ARMCC_VERSION) */


/*******************************************************************************
* Function Name: Bootloader_Start_c()
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
    Bootloader__main();
#elif defined (__GNUC__)
    Bootloader_Start_c();
#elif defined (__ICCARM__)
    Bootloader__iar_data_init3();
#endif /* defined(__ARMCC_VERSION) */
}


/*******************************************************************************
* Function Name: ConfigureSharedPins()
********************************************************************************
*
* Summary:
*   This function configures shared pins.
*
* Parameters:
*   None
*
*******************************************************************************/
void ConfigureSharedPins()
{
    Disconnect_LED_SetDriveMode(LED_1_DM_RES_UP);
    Advertising_LED_SetDriveMode(LED_1_DM_RES_UP);
    CapsLock_LED_SetDriveMode(LED_1_DM_RES_UP);
    Disconnect_LED_Write(LED_OFF);
    Advertising_LED_Write(LED_OFF);
    CapsLock_LED_Write(LED_OFF);
    Bootloader_Service_Activation_SetDriveMode(LED_1_DM_RES_UP);
    Bootloader_Service_Activation_Write(1);
}


/*******************************************************************************
* Function Name: BootloaderSwitch()
********************************************************************************
*
* Summary:
*   This function polls SW2 button and if it is pressed - shedules bootloader 
*   project launch. That action includes sotware reset.
*
* Parameters:
*   None
*
*******************************************************************************/
void BootloaderSwitch()
{
    if (Bootloader_Service_Activation_Read() == 0)
    {
        CyDelay(100);
        if (Bootloader_Service_Activation_Read() == 0)
        {
            DBG_PRINTF("Bootloader activation button pressed \r\n");
            DBG_PRINTF("Switching to bootloader application... \r\n");
            CyDelay(500);
            
            
            CyBle_Shutdown(); /* stop all ongoing activities */
            CyBle_ProcessEvents(); /* process all pending events */
            CyBle_SetState(CYBLE_STATE_STOPPED);
            CySysWdtUnlock();
            CySysWdtDisable(WDT_COUNTER_MASK);
            CyGlobalIntDisable;
            Bootloadable_Load();
        }
    }
}


/*******************************************************************************
* Function Name: ConfigureServices()
********************************************************************************
*
* Summary:
*   This function configures BLE component services to match required
*   configuration.
*
* Parameters:
*   None
*
*******************************************************************************/
void ConfigureServices()
{
    CyBle_GattsDisableAttribute(cyBle_btss.btServiceHandle);

#if defined(__ICCARM__)
    CyBle_GattsEnableAttribute(cyBle_hidss[0].serviceHandle);
    CyBle_GattsEnableAttribute(cyBle_diss.serviceHandle);
    CyBle_GattsEnableAttribute(cyBle_bass[0].serviceHandle);
    CyBle_GattsEnableAttribute(cyBle_scpss.serviceHandle);
#endif /* defined(__ICCARM__) */
}

/* [] END OF FILE */
