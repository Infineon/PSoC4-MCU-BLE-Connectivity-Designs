/*******************************************************************************
* File Name: debug.c
*
* Version: 1.20
*
* Description:
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
#include "debug.h"
#include "project.h"

#if (DEBUG_UART_ENABLED == YES)

#if defined(__ARMCC_VERSION)

/* For MDK/RVDS compiler revise fputc() for the printf() */
struct __FILE
{
    int handle;
};

enum
{
    STDIN_HANDLE,
    STDOUT_HANDLE,
    STDERR_HANDLE
} ;

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

    
    
#if DEBUG_UART_USE_PRINTF_FORMAT == NO

static uint32 divmod10(uint32 value, uint32 *reminder)
{
    uint32_t result = value / 10u;
    *reminder = value - result * 10u;
    return result;
}

void DBG_PRINT_DEC(uint32 value)
{
    char buf[12];
    uint32 len = 0;
    uint32 i = 0;
    do
    {
        uint32 rem;
        value = divmod10(value, &rem);
        buf[len] = '0' + rem;
        ++len;
    } while (value != 0);
    for (i = 0; i < len; ++i)
    {
        char tmp = buf[len - i - 1];
        buf[len - i - 1] = buf[i];
        buf[i] = tmp;
    }
    buf[len] = '\0';
    UART_PutString(buf);
}


void DBG_PRINT_HEX(uint32 a)
{
    int len = 0;
    do
    {
        if ((a >> 24) != 0)
        {
            break;
        }
        a <<= 8u;
        ++len;
    }
    while (len < 3);
    len = 4 - len;

    for ( ; len != 0; --len)
    {
        UART_PutHexByte(a >> 24);
        a <<= 8;
    }
}

#endif /* DEBUG_UART_USE_PRINTF_FORMAT == NO */

void DBG_PRINT_HEX_TEXT(uint32 hex, char *text)
{
    DBG_PRINT_HEX(hex);
    DBG_PRINT_TEXT(text);
}

void DBG_PRINT_HEX8_TEXT(uint32 hex, char *text)
{
    DBG_PRINT_HEX_BYTE(hex);
    DBG_PRINT_TEXT(text);
}

void DBG_PRINT_DEC_TEXT(uint32 dec, char *text)
{
    DBG_PRINT_DEC(dec);
    DBG_PRINT_TEXT(text);
}


#endif /* DEBUG_UART_ENABLE == YES */

/* [] END OF FILE */
