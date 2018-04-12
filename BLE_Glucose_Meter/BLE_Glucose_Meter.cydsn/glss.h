/*******************************************************************************
* File Name: glss.h
*
* Version 1.0
*
* Description:
*  GLS service related code header.
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

#if !defined(GLSS_H)
#define GLSS_H

#include "main.h"

#define CYBLE_GLS_REC_NUM           (11u) /* Number of records */
#define CYBLE_GLS_REC_STAT_OK       (0u)
#define CYBLE_GLS_REC_STAT_DELETED  (1u)

/* Glucose Measurement characteristic "Flags" bitfield flags */
#define CYBLE_GLS_GLMT_FLG_TOP (0x01u) /* Time Offset Present */
#define CYBLE_GLS_GLMT_FLG_GLC (0x02u) /* Glucose Concentration, Type and Sample Location Present */
#define CYBLE_GLS_GLMT_FLG_GCU (0x04u) /* Glucose Concentration Units, 1 = mol/L, 0 = kg/L */
#define CYBLE_GLS_GLMT_FLG_SSA (0x08u) /* Sensor Status Annunciation Present */
#define CYBLE_GLS_GLMT_FLG_CIF (0x10u) /* Context Information Follows */

/* Glucose Measurement characteristic "Sensor Status Annunciation" bitfield flags */
#define CYBLE_GLS_GLMT_SSA_BTL      (0x0001)  /* Device battery low at time of measurement */
#define CYBLE_GLS_GLMT_SSA_SMF      (0x0002)  /* Sensor malfunction or faulting at time of measurement */
#define CYBLE_GLS_GLMT_SSA_INSFNT   (0x0004)  /* Sample size for blood or control solution insufficient at time of measurement */
#define CYBLE_GLS_GLMT_SSA_STRERR   (0x0008)  /* Strip insertion error */
#define CYBLE_GLS_GLMT_SSA_STRTYP   (0x0010)  /* Strip type incorrect for device */
#define CYBLE_GLS_GLMT_SSA_SRH      (0x0020)  /* Sensor result higher than the device can process */
#define CYBLE_GLS_GLMT_SSA_SRL      (0x0040)  /* Sensor result lower than the device can process */
#define CYBLE_GLS_GLMT_SSA_STH      (0x0080)  /* Sensor temperature too high for valid test/result at time of measurement */
#define CYBLE_GLS_GLMT_SSA_STL      (0x0100)  /* Sensor temperature too low for valid test/result at time of measurement */
#define CYBLE_GLS_GLMT_SSA_SRI      (0x0200)  /* Sensor read interrupted because strip was pulled too soon at time of measurement */
#define CYBLE_GLS_GLMT_SSA_FLT      (0x0400)  /* General device fault has occurred in the sensor */
#define CYBLE_GLS_GLMT_SSA_TMFLT    (0x0800)  /* Time fault has occurred in the sensor and time may be inaccurate */

#define CYBLE_GLS_GLMT_TYPE_MASK    (0x0Fu) /* "Type" mask in the "tnsl" field */
#define CYBLE_GLS_GLMT_SL_MASK      (0xF0u) /* "Sample Location" mask in the "tnsl" field */
#define CYBLE_GLS_GLMT_SL_SHIFT     (4u)    /* "Sample Location" offset in the "tnsl" field */

/* Glucose Measurement Context characteristic "Flags" bitfield flags */
#define CYBLE_GLS_GLMC_FLG_CH       (0x01u) /* Carbohydrate ID And Carbohydrate Present */
#define CYBLE_GLS_GLMC_FLG_MEAL     (0x02u) /* Meal Present */
#define CYBLE_GLS_GLMC_FLG_TH       (0x04u) /* Tester-Health Present */
#define CYBLE_GLS_GLMC_FLG_EXER     (0x08u) /* Exercise Duration And Exercise Intensity Present */
#define CYBLE_GLS_GLMC_FLG_MEDC     (0x10u) /* Medication ID And Medication Present */
#define CYBLE_GLS_GLMC_FLG_MVU      (0x20u) /* Medication Value Units 1 = liters, 0 = kilograms */
#define CYBLE_GLS_GLMC_FLG_HBA1C    (0x40u) /* HbA1c Present */
#define CYBLE_GLS_GLMC_FLG_EXFLAG   (0x80u) /* Extended Flags Present */

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



typedef enum
{
    CYBLE_GLS_GLMT_TYPE_CWB = 1u,   /* Capillary Whole blood */
    CYBLE_GLS_GLMT_TYPE_CPL,        /* Capillary Plasma */
    CYBLE_GLS_GLMT_TYPE_VWB,        /* Venous Whole blood */
    CYBLE_GLS_GLMT_TYPE_VPL,        /* Venous Plasma */
    CYBLE_GLS_GLMT_TYPE_AWB,        /* Arterial Whole blood */
    CYBLE_GLS_GLMT_TYPE_APL,        /* Arterial Plasma */
    CYBLE_GLS_GLMT_TYPE_UWB,        /* Undetermined Whole blood */
    CYBLE_GLS_GLMT_TYPE_UPL,        /* Undetermined Plasma */
    CYBLE_GLS_GLMT_TYPE_ISF,        /* Interstitial Fluid */
    CYBLE_GLS_GLMT_TYPE_CS          /* Control Solution */
}CYBLE_GLS_GLMT_TYPE_T;

typedef enum
{
    CYBLE_GLS_GLMT_SL_FR = 1u,      /* Finger */
    CYBLE_GLS_GLMT_SL_AST,          /* Alternate Site Test */
    CYBLE_GLS_GLMT_SL_EL,           /* Earlobe */
    CYBLE_GLS_GLMT_SL_CS,           /* Control Solution */
    CYBLE_GLS_GLMT_SL_NAV = 0x0Fu   /* Sample Location value not available */
}CYBLE_GLS_GLMT_SL_T;

typedef uint16 sfloat;

typedef struct
{
    uint8  flags;
    uint16 seqNum;
    CYBLE_DATE_TIME_T baseTime;
    int16  timeOffset;  /* in minutes */
    sfloat gluConc;     /* Glucose Concentration, units depend on CYBLE_GLS_GLMT_FLG_GLCU flag */
    uint8  tnsl;        /* "Type" & "Sample Location" two nibble-type fields in one byte,
                        *  "Type" is less significant.
                        */
    uint16 ssa;         /* Sensor Status Annunciation */
}CYBLE_GLS_GLMT_T;

#define CYBLE_GLS_GLMC_FLG_CBID (0x01u) /* Carbohydrate ID And Carbohydrate Present */
#define CYBLE_GLS_GLMC_FLG_MEAL (0x02u) /* Meal Present */
#define CYBLE_GLS_GLMC_FLG_TNH  (0x04u) /* Tester-Health Present */
#define CYBLE_GLS_GLMC_FLG_EXR  (0x08u) /* Exercise Duration And Exercise Intensity Present */
#define CYBLE_GLS_GLMC_FLG_MED  (0x10u) /* Medication ID And Medication Present */
#define CYBLE_GLS_GLMC_FLG_MDU  (0x20u) /* Medication Value Units 0 = kilograms, 1 = liters */
#define CYBLE_GLS_GLMC_FLG_A1C  (0x40u) /* HbA1c Present */
#define CYBLE_GLS_GLMC_FLG_EXT  (0x80u) /* Extended Flags Present */

#define CYBLE_GLS_GLMC_TESTER_MASK  (0x0Fu) /* "Tester" mask in the "tnh" field */
#define CYBLE_GLS_GLMC_HEALTH_MASK  (0xF0u) /* "Health" mask in the "tnh" field */
#define CYBLE_GLS_GLMC_HEALTH_SHIFT (4u)    /* "Health" offset in the "tnh" field */

typedef enum
{
    CYBLE_GLS_GLMC_CBID_BREAKFAST = 1u, /* Breakfast */
    CYBLE_GLS_GLMC_CBID_LUNCH,          /* Lunch */
    CYBLE_GLS_GLMC_CBID_DINNER,         /* Dinner */
    CYBLE_GLS_GLMC_CBID_SNACK,          /* Snack */
    CYBLE_GLS_GLMC_CBID_DRINK,          /* Drink */
    CYBLE_GLS_GLMC_CBID_SUPPER,         /* Supper */
    CYBLE_GLS_GLMC_CBID_BRUNCH          /* Brunch */
}CYBLE_GLS_GLMC_CBID_T;

typedef enum
{
    CYBLE_GLS_GLMC_MEAL_PRE = 1u,   /* Preprandial (before meal) */
    CYBLE_GLS_GLMC_MEAL_POST,       /* Postprandial (after meal) */
    CYBLE_GLS_GLMC_MEAL_FAST,       /* Fasting */
    CYBLE_GLS_GLMC_MEAL_CAS,        /* Casual (snacks, drinks, etc.) */
    CYBLE_GLS_GLMC_MEAL_BED         /* Bedtime */
}CYBLE_GLS_GLMC_MEAL_T;

typedef enum
{
    CYBLE_GLS_GLMC_TESTER_SELF = 1u,    /* Self */
    CYBLE_GLS_GLMC_TESTER_HCP,          /* Health Care Professional */
    CYBLE_GLS_GLMC_TESTER_LAB,          /* Lab test */
    CYBLE_GLS_GLMC_TESTER_NAV = 0x0Fu   /* Tester value not available */
}CYBLE_GLS_GLMC_TESTER_T;

typedef enum
{
    CYBLE_GLS_GLMC_HEALTH_MIN = 1u,     /* Minor health issues */
    CYBLE_GLS_GLMC_HEALTH_MAJ,          /* Major health issues */
    CYBLE_GLS_GLMC_HEALTH_DM,           /* During menses */
    CYBLE_GLS_GLMC_HEALTH_US,           /* Under stress */
    CYBLE_GLS_GLMC_HEALTH_NO,           /* No health issues */
    CYBLE_GLS_GLMC_HEALTH_NAV = 0x0Fu   /* Health value not available */
}CYBLE_GLS_GLMC_HEALTH_T;

typedef enum
{
    CYBLE_GLS_GLMC_MEDID_RAI = 1u, /* Rapid acting insulin */
    CYBLE_GLS_GLMC_MEDID_SAI,      /* Short acting insulin */
    CYBLE_GLS_GLMC_MEDID_IAI,      /* Intermediate acting insulin */
    CYBLE_GLS_GLMC_MEDID_LAI,      /* Long acting insulin */
    CYBLE_GLS_GLMC_MEDID_PMI,      /* Pre-mixed insulin */
}CYBLE_GLS_GLMC_MEDID_T;

typedef struct
{
    uint8  flags;
    uint16 seqNum;
    uint8  exFlags;
    uint8  cbId;    /* Carbohydrate ID */
    sfloat cbhdr;   /* Carbohydrate */
    uint8  meal;
    uint8  tnh;     /* "Tester" & "Health" two nibble-type fields in one byte,
                     * "Tester" is less significant.
                     */
    uint16 exDur;   /* Exercise Duration (in seconds) */
    uint8  exInt;   /* Exercise Intensity (in percents) */
    uint8  medId;   /* Medication ID */
    sfloat medic;   /* Medication */
    sfloat hba1c;   /* HbA1c (glycated hemoglobin) */
}CYBLE_GLS_GLMC_T;

/* Glucose Feature characteristic value type */
typedef enum
{
    CYBLE_GLS_GLFT_LOW_BATTERY      = 0x0001u,   /* Low Battery Detection During Measurement Supported */
    CYBLE_GLS_GLFT_SNS_MLFNCTN      = 0x0002u,   /* Sensor Malfunction Detection Supported */
    CYBLE_GLS_GLFT_SNS_SMPL_SZ      = 0x0004u,   /* Sensor Sample Size Supported */
    CYBLE_GLS_GLFT_SNS_STRP_INS_ERR = 0x0008u,   /* Sensor Strip Insertion Error Detection Supported */
    CYBLE_GLS_GLFT_SNS_STRP_TYP_ERR = 0x0010u,   /* Sensor Strip Type Error Detection Supported */
    CYBLE_GLS_GLFT_SNS_RSLT_HI_LO   = 0x0020u,   /* Sensor Result High-Low Detection Supported */
    CYBLE_GLS_GLFT_SNS_TEMP_HI_LO   = 0x0040u,   /* Sensor Temperature High-Low Detection Supported */
    CYBLE_GLS_GLFT_SNS_READ_INT     = 0x0080u,   /* Sensor Read Interrupt Detection Supported */
    CYBLE_GLS_GLFT_GNRL_DEV_FAULT   = 0x0100u,   /* General Device Fault Supported */
    CYBLE_GLS_GLFT_TIME_FAULT       = 0x0200u,   /* Time Fault Supported */
    CYBLE_GLS_GLFT_MLTP_BOND        = 0x0400u    /* Multiple Bond Supported */
} CYBLE_GLS_GLFT_T;

/* Opcode of the Record Access Control Point characteristic value type */
typedef enum
{
    CYBLE_GLS_RACP_OPC_RESERVED = 0u,   /* Reserved for future use (Operator: N/A) */
    CYBLE_GLS_RACP_OPC_REPORT_REC,      /* Report stored records (Operator: Value from Operator Table) */
    CYBLE_GLS_RACP_OPC_DELETE_REC,      /* Delete stored records (Operator: Value from Operator Table) */
    CYBLE_GLS_RACP_OPC_ABORT_OPN,       /* Abort operation (Operator: Null 'value of 0x00 from Operator Table') */
    CYBLE_GLS_RACP_OPC_REPORT_NUM_REC,  /* Report number of stored records (Operator: Value from Operator Table) */
    CYBLE_GLS_RACP_OPC_NUM_REC_RSP,     /* Number of stored records response (Operator: Null 'value of 0x00 from Operator Table') */
    CYBLE_GLS_RACP_OPC_RSP_CODE,        /* Response Code (Operator: Null 'value of 0x00 from Operator Table') */

} CYBLE_GLS_RACP_OPC_T;

/* Operator of the Record Access Control Point characteristic value type */
typedef enum
{
    CYBLE_GLS_RACP_OPR_NULL = 0u,  /* Null */
    CYBLE_GLS_RACP_OPR_ALL,        /* All records */
    CYBLE_GLS_RACP_OPR_LESS,       /* Less than or equal to */
    CYBLE_GLS_RACP_OPR_GREAT,      /* Greater than or equal to */
    CYBLE_GLS_RACP_OPR_WITHIN,     /* Within range of (inclusive) */
    CYBLE_GLS_RACP_OPR_FIRST,      /* First record (i.e. oldest record) */
    CYBLE_GLS_RACP_OPR_LAST        /* Last record (i.e. most recent record) */
} CYBLE_GLS_RACP_OPR_T;

/* Operand of the Record Access Control Point characteristic value type */
typedef enum
{
    CYBLE_GLS_RACP_OPD_NA = 0u,     /* N/A */
    CYBLE_GLS_RACP_OPD_1,           /* Filter parameters (as appropriate to Operator and Service) */
    CYBLE_GLS_RACP_OPD_2,           /* Filter parameters (as appropriate to Operator and Service) */
    CYBLE_GLS_RACP_OPD_NO_INCL,     /* Not included */
    CYBLE_GLS_RACP_OPD_4,           /* Filter parameters (as appropriate to Operator and Service) */
    CYBLE_GLS_RACP_OPD_NUM_REC,     /* Number of Records (Field size defined per service) */
    CYBLE_GLS_RACP_OPD_RSP,         /* Request Op Code, Response Code Value */

} CYBLE_GLS_RACP_OPD_T;

/* Operand Response Code Values of the Record Access Control Point characteristic value type */
typedef enum
{
    CYBLE_GLS_RACP_RSP_NA = 0u,     /* N/A */
    CYBLE_GLS_RACP_RSP_SUCCESS,     /* Normal response for successful operation */
    CYBLE_GLS_RACP_RSP_UNSPRT_OPC,  /* Normal response if unsupported Op Code is received */
    CYBLE_GLS_RACP_RSP_INV_OPR,     /* Normal response if Operator received does not meet the
                                     * requirements of the service (e.g. Null was expected)
                                     */
    CYBLE_GLS_RACP_RSP_UNSPRT_OPR,  /* Normal response if unsupported Operator is received */
    CYBLE_GLS_RACP_RSP_INV_OPD,     /* Normal response if Operand received does not meet the
                                     * requirements of the service
                                     */
    CYBLE_GLS_RACP_RSP_NO_REC,      /* Normal response if request to report stored records
                                     * or request to delete stored records resulted in no
                                     * records meeting criteria.
                                     */
    CYBLE_GLS_RACP_RSP_UNSUCCESS,   /* Normal response if request for Abort cannot be completed */
    CYBLE_GLS_RACP_RSP_NO_COMPL,    /* Normal response if request for Abort cannot be completed */
    CYBLE_GLS_RACP_RSP_UNSPRT_OPD   /* Normal response if unsupported Operand is received */

} CYBLE_GLS_RACP_RSP_T;



/***************************************
*      Function prototypes
***************************************/
void GlsInit(void);
void GlsCallBack(uint32 event, void* eventParam);
void GlsProcess(void);

/* Internal functions */
void GlsNtf(uint8 num);
void GlsInd(void);
CYBLE_DATE_TIME_COMP_T TimeComparison(CYBLE_DATE_TIME_T glmtTime, CYBLE_DATE_TIME_T userFacingTime);

/***************************************
*      External data references
***************************************/

extern CYBLE_GLS_GLMT_T glsGlucose[CYBLE_GLS_REC_NUM];
extern CYBLE_GLS_GLMC_T glsGluCont[CYBLE_GLS_REC_NUM];


#endif /* GLSS_H  */

/* [] END OF FILE */
