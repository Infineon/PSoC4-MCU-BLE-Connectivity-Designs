/*******************************************************************************
* File Name: debug.c
*
* Version: 1.40
*
* Description:
*  This file contains functions for printf functionality.
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation.  All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include "project.h"
#include "common.h"

#if defined(__ARMCC_VERSION)
    
/* For MDK/RVDS compiler revise fputc function for printf functionality */
struct __FILE 
{
    int handle;  
};

enum 
{
    STDIN_HANDLE,
    STDOUT_HANDLE,
    STDERR_HANDLE
};

FILE __stdin = {STDIN_HANDLE};
FILE __stdout = {STDOUT_HANDLE};
FILE __stderr = {STDERR_HANDLE};

int fputc(int ch, FILE *file) 
{
    int ret = EOF;

    switch( file->handle )
    {
        case STDOUT_HANDLE:
            UART_PutChar(ch);
            ret = ch ;
            break ;

        case STDERR_HANDLE:
            ret = ch ;
            break ;

        default:
            file = file;
            break ;
    }
    return ret ;
}

#elif defined (__ICCARM__)      /* IAR */

/* For IAR compiler revise __write() function for printf functionality */
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
    size_t nChars = 0;

    if (buffer == 0)
    {
        /*
         * This means that we should flush internal buffers.  Since we
         * don't we just return.  (Remember, "handle" == -1 means that all
         * handles should be flushed.)
         */
        return (0);
    }

    for (/* Empty */; size != 0; --size)
    {
        UART_PutChar(*buffer);
        ++buffer;
        ++nChars;
    }

    return (nChars);
}

#else  /* (__GNUC__)  GCC */

/* For GCC compiler revise _write() function for printf functionality */
int _write(int file, char *ptr, int len)
{
    int i;
    file = file;
    for (i = 0; i < len; i++)
    {
        UART_PutChar(*ptr);
        ++ptr;
    }
    return len;
}


#endif  /* (__ARMCC_VERSION) */   

void ShowValue(CYBLE_GATT_VALUE_T *value)
{
    int16 i;
    
    for(i = 0; i < value->len; i++)
    {
        DBG_PRINTF("%2.2x ", value->val[i]);
    }
    DBG_PRINTF("\r\n");
}


void Set32ByPtr(uint8 ptr[], uint32 value)
{
    ptr[0u] = (uint8) value;
    ptr[1u] = (uint8) (value >> 8u);    
    ptr[2u] = (uint8) (value >> 16u);    
    ptr[3u] = (uint8) (value >> 24u);
}


/*******************************************************************************
* Function Name: ShowError()
********************************************************************************
*
* Summary:
*   Shows error condition: Turn On all leds - white colour will indicate error.
*
*******************************************************************************/
void ShowError(void)
{
    Disconnect_LED_Write(LED_ON);
    Advertising_LED_Write(LED_ON);
    CapsLock_LED_Write(LED_ON);
    /* Halt CPU in Debug mode */
    CYASSERT(0u != 0u);
}

/* [] END OF FILE */
