/*******************************************************************************
* File Name: bcs.h
* Version 1.0
*
* Description:
*  Contains the function prototypes and constants used by Body Composition 
*  Service.
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "BLE.h"


/***************************************
*       Data Struct Definition
***************************************/
/* BCS Measurement value type */
CYBLE_CYPACKED typedef struct
{
    /* The flags have been included to support future extensions of the BCS
    * measurement descriptor.
    */
    uint16 flags;
    uint16 bodyFatPercentage;
    uint16 year;
    uint8  month;
    uint8  day;
    uint8  hour;
    uint8  minutes;
    uint8  seconds;
    uint8  userId;
    uint16 basalMetabolism;
    uint16 musclePercentage;
    uint16 muscleMassKg;
    uint16 muscleMassLb;
    uint16 fatFreeMassKg;
    uint16 fatFreeMassLb;
    uint16 softLeanMassKg;
    uint16 softLeanMassLb;
    uint16 bodyWatherMassKg;
    uint16 bodyWatherMassLb;
    uint16 impedance;
    uint16 weightKg;
    uint16 weightLb;
    uint16 heightM;
    uint16 heightIn;
}CYBLE_CYPACKED_ATTR BCS_MEASUREMENT_VALUE_T;


/***************************************
*          Constants
***************************************/
#define BCS_BC_MEASUREMENT_MAX_DATA_SIZE           (42u)
#define BCS_BC_FEATURE_CHAR_SIZE                   (4u)
#define BCS_FLAGS_COUNT                            (13u)

/* Body Composition Feature constants */
#define BCS_TIME_STAMP_SUPPORTED                   (0x00000001ul)
#define BCS_MULTIPLE_USERS_SUPPORTED               (0x00000002ul)
#define BCS_BASAL_METABOLISM_SUPPORTED             (0x00000004ul)
#define BCS_MUSCLE_PERCENTAGE_SUPPORTED            (0x00000008ul)
#define BCS_MUSCLE_MASS_SUPPORTED                  (0x00000010ul)
#define BCS_FAT_FREE_MASS_SUPPORTED                (0x00000020ul)
#define BCS_SOFT_LEAN_MASS_SUPPORTED               (0x00000040ul)
#define BCS_BODY_WATER_MASS_SUPPORTED              (0x00000080ul)
#define BCS_IMPENDANCE_SUPPORTED                   (0x00000100ul)
#define BCS_WEIGHT_SUPPORTED                       (0x00000200ul)
#define BCS_HEIGHT_SUPPORTED                       (0x00000400ul)
#define BCS_WEIGHT_MASK                            (0x00007800ul)
#define BCS_HEIGHT_MASK                            (0x00028000ul)

#define BCS_MULTIPLE_USERS_SHIFT                   (1u)
#define BCS_BASAL_METABOLISM_SHIFT                 (2u)
#define BCS_MUSCLE_PERCENTAGE_SHIFT                (3u)
#define BCS_MUSCLE_MASS_SHIFT                      (4u)
#define BCS_FAT_FREE_MASS_SHIFT                    (5u)
#define BCS_SOFT_LEAN_MASS_SHIFT                   (6u)
#define BCS_BODY_WATER_MASS_SHIFT                  (7u)
#define BCS_IMPENDANCE_SHIFT                       (8u)
#define BCS_WEIGHT_SHIFT                           (9u)
#define BCS_HEIGHT_SHIFT                           (10u)
#define BCS_WEIGHT_MASK_SHIFT                      (11u)
#define BCS_HEIGHT_MASK_SHIFT                      (15u)

/* Body Composition Measurement Flags constants */
#define BCS_MEASUREMENT_UNITS_IMPERIAL             (0x0001u)
#define BCS_TIME_STAMP_PRESENT                     (0x0002u)
#define BCS_USER_ID_PRESENT                        (0x0004u)
#define BCS_BASAL_METABOLISM_PRESENT               (0x0008u)
#define BCS_MUSCLE_PERCENTAGE_PRESENT              (0x0010u)
#define BCS_MUSCLE_MASS_PRESENT                    (0x0020u)
#define BCS_FAT_FREE_MASS_PRESENT                  (0x0040u)
#define BCS_SOFT_LEAN_MASS_PRESENT                 (0x0080u)
#define BCS_BODY_WATER_MASS_PRESENT                (0x0100u)
#define BCS_IMPEDANCE_PRESENT                      (0x0200u)
#define BCS_WEIGHT_PRESENT                         (0x0400u)
#define BCS_HEIGHT_PRESENT                         (0x0800u)
#define BCS_MULTIPLE_PACKET_MEASUREMENT            (0x1000u)

/* Return constants */
#define BCS_RET_SUCCESS                            (0u)
#define BCS_RET_FAILURE                            (1u)


/***************************************
*        Function Prototypes
***************************************/
void BcsInit(void);
void BcsCallBack(uint32 event, void *eventParam);
uint8 BcsPackIndicationData(uint8 *pData, uint8 *length, BCS_MEASUREMENT_VALUE_T *bMeasurement);


/***************************************
* External data references
***************************************/
extern uint8                      isBcsIndicationEnabled;
extern uint8                      isBcsIndicationPending;
extern BCS_MEASUREMENT_VALUE_T    bodyMeasurement[];
extern uint8                      bcsIndData[];
extern uint32                     bcsFeature;


/* [] END OF FILE */
