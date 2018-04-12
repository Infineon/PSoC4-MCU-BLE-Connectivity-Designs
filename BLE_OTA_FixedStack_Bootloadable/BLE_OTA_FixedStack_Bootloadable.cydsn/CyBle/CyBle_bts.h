/***************************************************************************//**
* \file CYBLE_bts.h
* \version 3.30
* 
* \brief
*  Contains the function prototypes and constants for the Bootloader Service.
* 
********************************************************************************
* \copyright
* Copyright 2015-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BLE_CYBLE_BTS_H)
#define CY_BLE_CYBLE_BTS_H

#include "CyBle_gatt.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Maximum supported Custom Services */
#define CYBLE_BT_SERVICE_COUNT                       (0x01u)
#define CYBLE_BT_SERVICE_CHAR_COUNT                  (0x01u)
#define CYBLE_BT_SERVICE_CHAR_DESCRIPTORS_COUNT      (0x01u)
    

/***************************************
* Data Struct Definition
***************************************/

/** \cond IGNORE */

/* Contains information about Bootloader Characteristic structure */
typedef struct
{
    /* Bootloader Characteristic handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T btServiceCharHandle;
    /* Bootloader Characteristic Descriptors handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T btServiceCharDescriptors[CYBLE_BT_SERVICE_CHAR_DESCRIPTORS_COUNT];
} CYBLE_BTS_INFO_T;

/* Structure with Bootloader Service attribute handles. */
typedef struct
{
    /* Handle of a Bootloader Service */
    CYBLE_GATT_DB_ATTR_HANDLE_T btServiceHandle;
    
    /* Information about Bootloader Characteristics */
    CYBLE_BTS_INFO_T btServiceInfo[CYBLE_BT_SERVICE_CHAR_COUNT];
} CYBLE_BTSS_T;

/** \endcond */

/***************************************
* API Constants
***************************************/

#define CYBLE_BTS_COMMAND_DATA_LEN_OFFSET                 (2u)
#define CYBLE_BTS_COMMAND_CONTROL_BYTES_NUM               (7u)
#define CYBLE_BTS_MTU_OVERHEAD                            (5u)
#define CYBLE_BTS_COMMAND_MAX_LENGTH                      (263u)

#define CYBLE_BTS_WAIT_1_MS                               (1u)


/***************************************
* Function Prototypes
***************************************/
void CyBLE_CyBtldrCommStart(void);
void CyBLE_CyBtldrCommStop(void);
void CyBLE_CyBtldrCommReset(void);
cystatus CyBLE_CyBtldrCommWrite(const uint8 data[], uint16 size, uint16 *count, uint8 timeOut);
cystatus CyBLE_CyBtldrCommRead(uint8 data[], uint16 size, uint16 *count, uint8 timeOut);


/***************************************
* Private Function Prototypes
***************************************/
/** \cond IGNORE */
CYBLE_GATT_ERR_CODE_T CyBle_BtsWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_BtssExecuteWriteRequestEventHandler(CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam);
void CyBle_BtssPrepareWriteRequestEventHandler(const CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam);
/** \endcond */


/***************************************
* External data references
***************************************/
extern uint8  cyBle_cmdReceivedFlag;
extern uint16 cyBle_cmdLength;
extern uint8 *cyBle_btsBuffPtr;

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (0u != (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_CyBle))
    #define CyBtldrCommStart   CyBLE_CyBtldrCommStart
    #define CyBtldrCommStop    CyBLE_CyBtldrCommStop
    #define CyBtldrCommReset   CyBLE_CyBtldrCommReset
    #define CyBtldrCommWrite   CyBLE_CyBtldrCommWrite
    #define CyBtldrCommRead    CyBLE_CyBtldrCommRead
#endif /* defined(CYDEV_BOOTLOADER_IO_COMP */

extern const CYBLE_BTSS_T cyBle_btss;

#endif /* !defined(CY_BLE_CYBLE_BTS_H) */


/* [] END OF FILE */
