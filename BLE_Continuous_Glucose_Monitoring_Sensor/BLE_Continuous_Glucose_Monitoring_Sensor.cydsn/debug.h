/*******************************************************************************
* File Name: debug.h
*
* Version 1.0
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project.
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(DBG_H)
#define DBG_H
    
#include "main.h"

    
#define ENABLED                             (1u)
#define DISABLED                            (0u)


/***************************************
* Conditional Compilation Parameters
***************************************/
#define DEBUG_UART_ENABLED                  ENABLED

/***************************************
* Conditional Compilation Parameters
***************************************/
    
/***************************************
*        Macros
***************************************/
#if (DEBUG_UART_ENABLED == ENABLED)
    #define DBG_PRINTF(...)                 (printf(__VA_ARGS__))
    #define UART_DEB_PUT_CHAR(...)          (UART_DEB_UartPutChar(__VA_ARGS__))
    #define UART_DEB_GET_CHAR(...)          (UART_DEB_UartGetChar(__VA_ARGS__))
    #define UART_DEB_GET_TX_BUFF_SIZE(...)  (UART_DEB_SpiUartGetTxBufferSize(__VA_ARGS__))
    #define UART_START(...)                 (UART_DEB_Start(__VA_ARGS__))
#else
    #define DBG_PRINTF(...)
    #define UART_DEB_PUT_CHAR(...)
    #define UART_DEB_GET_CHAR(...)          (0u)
    #ifndef UART_DEB_GET_TX_FIFO_SR_VALID
        #define UART_DEB_GET_TX_FIFO_SR_VALID   (0u)
    #endif
    #define UART_DEB_GET_TX_BUFF_SIZE(...)
    #define UART_START(...)
#endif /* (DEBUG_UART_ENABLED == ENABLED) */

/***************************************
*      API Function Prototypes
***************************************/
int _write(int file, char *ptr, int len);
void PrintStackVersion(void);
void PrintState(void);
void PrintApiResult(void);


/***************************************
*        Macros
***************************************/
#if (DEBUG_UART_ENABLED == ENABLED)
    #define DBG_PRINTF(...)          (printf(__VA_ARGS__))
#else
    #define DBG_PRINTF(...)
#endif /* (DEBUG_UART_ENABLED == ENABLED) */


#endif /* DBG_H */

/* [] END OF FILE */
