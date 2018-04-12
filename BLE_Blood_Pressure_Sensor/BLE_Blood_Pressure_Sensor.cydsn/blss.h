/*******************************************************************************
* File Name: blss.h
*
* Version 1.0
*
* Description:
*  BLS service related code header.
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(BLSS_H)
#define BLSS_H

#include "main.h"


#define IND (0x01u)
#define NTF (0x02u)
    
#define SIM_BLS_DLT     (10u)
#define SIM_UNIT_MAX    (59u)  /* seconds in minute */
#define SIM_ICF_MAX     (200u)
#define SIM_ICF_MSK     (0x07)
#define SIM_BPM_SYS_MIN (100u)
#define SIM_BPM_DIA_MIN (60u)
#define SIM_BPM_MSK     (0x38)
#define BLS_TIMEOUT     (2u)

/* Blood Pressure Measurement characteristic "Flags" bitfield flags */
#define CYBLE_BLS_BPM_FLG_BPU (0x01u) /* Blood Pressure Units 0 = mmHg, 1 = kPa */
#define CYBLE_BLS_BPM_FLG_TSP (0x02u) /* Time Stamp */
#define CYBLE_BLS_BPM_FLG_PRT (0x04u) /* Pulse Rate */
#define CYBLE_BLS_BPM_FLG_UID (0x08u) /* User ID */
#define CYBLE_BLS_BPM_FLG_MST (0x10u) /* Measurement Status */

/* Blood Pressure Measurement characteristic "Measurement Status" bitfield flags */
#define CYBLE_BLS_BPM_MST_BMD      (0x0001)  /* Body Movement Detection */
#define CYBLE_BLS_BPM_MST_CFD      (0x0002)  /* Cuff Fit Detection */
#define CYBLE_BLS_BPM_MST_IPD      (0x0004)  /* Irregular Pulse Detection */
#define CYBLE_BLS_BPM_MST_PRW      (0x0000)  /* Pulse Rate Range Detection: Pulse rate is within the range */
#define CYBLE_BLS_BPM_MST_PRH      (0x0008)  /* Pulse Rate Range Detection: Pulse rate exceeds upper limit */
#define CYBLE_BLS_BPM_MST_PRL      (0x0010)  /* Pulse Rate Range Detection: Pulse rate is less than lower limit */
#define CYBLE_BLS_BPM_MST_PRM      (0x0018)  /* Pulse Rate Range Detection Mask */
#define CYBLE_BLS_BPM_MST_MPD      (0x0020)  /* Measurement Position Detection */


typedef struct
{
    uint16 year;
    uint8  month;
    uint8  day;
    uint8  hours;
    uint8  minutes;
    uint8  seconds;
}CYBLE_DATE_TIME_T;

typedef enum
{
    CYBLE_TIME_EQUAL,
    CYBLE_TIME_LESS,
    CYBLE_TIME_GREAT
}CYBLE_DATE_TIME_COMP_T;

typedef uint16 sfloat; /* Placeholder for SFLOAT */

typedef struct
{
    uint8  flags;
    sfloat sys;             /* Systolic */
    sfloat dia;             /* Diastolic */
    sfloat map;             /* Mean Arterial Pressure */
    CYBLE_DATE_TIME_T time; /* Time Stamp */
    sfloat prt;             /* Pulse Rate */
    uint8  uid;             /* User ID */
    uint16 mst;             /* Measurement Status */
}CYBLE_BLS_BPM_T;

/* Blood Pressure Feature characteristic value type */
typedef enum
{
    CYBLE_BLS_BPF_BMD = 0x0001u,    /* Body Movement Detection Support */
    CYBLE_BLS_BPF_CFD = 0x0002u,    /* Cuff Fit Detection Support */
    CYBLE_BLS_BPF_IPD = 0x0004u,    /* Irregular Pulse Detection Support */
    CYBLE_BLS_BPF_PRD = 0x0008u,    /* Pulse Rate Range Detection Support */
    CYBLE_BLS_BPF_MPD = 0x0010u,    /* Measurement Position Detection Support */
    CYBLE_BLS_BPF_MBS = 0x0020u,    /* Multiple Bond Support */
} CYBLE_BLS_BPF_T;


/***************************************
*      Function prototypes
***************************************/

void BlsCallBack(uint32 event, void* eventParam);
void BlsInit(void);
void BlsSimulate(void);
void BlsInd(uint8 num);
void BlsNtf(uint8 num);

/***************************************
*      External data references
***************************************/

extern uint16 feature;



#endif /* BLSS_H  */

/* [] END OF FILE */
