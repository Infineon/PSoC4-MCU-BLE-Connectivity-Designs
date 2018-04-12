/*******************************************************************************
* File Name: wss.h
* Version 1.0
*
* Description:
*  Contains the function prototypes and constants used by Weight Scale Service.
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>


/***************************************
*       Data Struct Definition
***************************************/
/* WSS Measurement value type */
CYBLE_CYPACKED typedef struct
{
    uint8 flags;
    uint16 weightKg;
    uint16 weightLb;
    uint16 year;
    uint8  month;
    uint8  day;
    uint8  hour;
    uint8  minutes;
    uint8  seconds;
    uint8  userId;
    uint16 bmi;
    uint16 heightM;
    uint16 heightIn;
}CYBLE_CYPACKED_ATTR WSS_MEASUREMENT_VALUE_T;


/***************************************
*          Constants
***************************************/
#define WSS_MEASUREMENT_UNITS_IMPERIAL                  (0x01u)
#define WSS_MEASUREMENT_TIME_STAMP_PRESENT              (0x02u)
#define WSS_USER_ID_PRESENT                             (0x04u)
#define WSS_BMI_AND_HEIGHT_PRESENT                      (0x08u)

#define WSS_FLAGS_COUNT                                 (4u)
#define WSS_WEIGHT_FEATURE_CHAR_SIZE                    (4u)
#define WSS_WS_MEASUREMENT_MAX_DATA_SIZE                (19u)

#define WSS_WEIGHT_FEATURE_BYTE1                        (0u)
#define WSS_WEIGHT_FEATURE_BYTE2                        (1u)
#define WSS_WEIGHT_FEATURE_BYTE3                        (2u)
#define WSS_WEIGHT_FEATURE_BYTE4                        (3u)

#define WSS_DEFAULT_USER_ID                             (0u)

/* Return constants */
#define WSS_RET_SUCCESS                                 (0u)
#define WSS_RET_FAILURE                                 (1u)

#define WSS_SENSOR_TIMER_PERIOD                         (7u)


/***************************************
*        Function Prototypes
***************************************/
void WssInit(void);
void WssCallBack(uint32 event, void *eventParam);
uint8 WssPackIndicationData(uint8 *pData, uint8 *length, WSS_MEASUREMENT_VALUE_T *wMeasurement);


/***************************************
* External data references
***************************************/
extern uint8                      isWssIndicationEnabled;
extern uint8                      isWssIndicationPending;
extern WSS_MEASUREMENT_VALUE_T    weightMeasurement[];
extern uint8                      wssIndData[];


/* [] END OF FILE */
