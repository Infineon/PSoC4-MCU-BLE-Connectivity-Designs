/*******************************************************************************
* File Name: cgmss.h
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

#if !defined(CGMSS_H)
#define CGMSS_H

#include "main.h"
    
typedef uint16 sfloat;
#define SFLOAT_NAN  (0x07ffu) /* not a number */
#define SFLOAT_NRES (0x0800u) /* not at this resolution */
#define SFLOAT_PINF (0x07feu) /* + infinity */
#define SFLOAT_NINF (0x0802u) /* - infinity */
#define SFLOAT_RSRV (0x0801u) /* reserved for future use */

#define REC_STATUS_OK      (0u)
#define REC_STATUS_DELETED (1u)
#define REC_NUM            (3u)

#define CGMS_FLAG_SOCP (0x01u)
#define CGMS_FLAG_RACP (0x02u)

#define CYBLE_CGMS_CRC_SEED (0xFFFFu) /* CRC-CCITT initial seed value */
#define CYBLE_CGMS_CRC_POLY (0x8408u) /* CRC-CCITT polynomial is D16+D12+D5+1 in reverse order */

#define CYBLE_CGMS_SSTM_SIZE    (9u)
#define CYBLE_CGMS_CRC_SIZE     (2u)

/* CGM Measurement characteristic "Flags" bitfield flags */
#define CYBLE_CGMS_GLMT_FLG_TI (0x01u) /* CGM Trend Information Present */
#define CYBLE_CGMS_GLMT_FLG_QA (0x02u) /* CGM Quality Present */
#define CYBLE_CGMS_GLMT_FLG_WG (0x20u) /* Sensor Status Annunciation Field, Warning-Octet present */
#define CYBLE_CGMS_GLMT_FLG_CT (0x40u) /* Sensor Status Annunciation Field, Cal/Temp-Octet present */
#define CYBLE_CGMS_GLMT_FLG_ST (0x80u) /* Sensor Status Annunciation Field, Status-Octet present */

/* CGM Measurement characteristic "Sensor Status Annunciation" bitfield flags */
#define CYBLE_CGMS_GLMT_SSA_SS      (0x00000001u)  /* Session Stopped */
#define CYBLE_CGMS_GLMT_SSA_BL      (0x00000002u)  /* Device Battery Low */
#define CYBLE_CGMS_GLMT_SSA_TE      (0x00000004u)  /* Sensor type incorrect for device */
#define CYBLE_CGMS_GLMT_SSA_SM      (0x00000008u)  /* Sensor malfunction */
#define CYBLE_CGMS_GLMT_SSA_DA      (0x00000010u)  /* Device Specific Alert */
#define CYBLE_CGMS_GLMT_SSA_DF      (0x00000020u)  /* General device fault has occurred in the sensor */
#define CYBLE_CGMS_GLMT_SSA_WGM     (0x0000003Fu)  /* Warning-Octet mask */
    
#define CYBLE_CGMS_GLMT_SSA_TS      (0x00000100u)  /* Time synchronization between sensor and collector required */
#define CYBLE_CGMS_GLMT_SSA_CN      (0x00000200u)  /* Calibration not allowed */
#define CYBLE_CGMS_GLMT_SSA_CR      (0x00000400u)  /* Calibration recommended */
#define CYBLE_CGMS_GLMT_SSA_CL      (0x00000800u)  /* Calibration required */
#define CYBLE_CGMS_GLMT_SSA_TH      (0x00001000u)  /* Sensor Temperature too high for valid test/result at time of measurement */
#define CYBLE_CGMS_GLMT_SSA_TL      (0x00002000u)  /* Sensor temperature too low for valid test/result at time of measurement */
#define CYBLE_CGMS_GLMT_SSA_CTM     (0x00003F00u)  /* Cal/Temp-Octet mask */
#define CYBLE_CGMS_GLMT_SSA_CTS     (8u)           /* Cal/Temp-Octet shift */
    
#define CYBLE_CGMS_GLMT_SSA_RL      (0x00010000u)  /* Sensor result lower than the Patient Low level */
#define CYBLE_CGMS_GLMT_SSA_RH      (0x00020000u)  /* Sensor result higher than the Patient High level */
#define CYBLE_CGMS_GLMT_SSA_HL      (0x00040000u)  /* Sensor result lower than the Hypo level */
#define CYBLE_CGMS_GLMT_SSA_HH      (0x00080000u)  /* Sensor result higher than the Hyper level */
#define CYBLE_CGMS_GLMT_SSA_SD      (0x00100000u)  /* Sensor Rate of Decrease exceeded */
#define CYBLE_CGMS_GLMT_SSA_SI      (0x00200000u)  /* Sensor Rate of Increase exceeded */
#define CYBLE_CGMS_GLMT_SSA_DL      (0x00400000u)  /* Sensor result lower than the device can process */
#define CYBLE_CGMS_GLMT_SSA_DH      (0x00800000u)  /* Sensor result higher than the device can process */
#define CYBLE_CGMS_GLMT_SSA_STM     (0x00FF0000u)  /* Status-Octet mask */
#define CYBLE_CGMS_GLMT_SSA_STS     (16u)          /* Status-Octet shift */

/* Glucose Measurement Context characteristic "Flags" bitfield flags */
#define CYBLE_CGMS_CGMT_FLG_CH       (0x01u) /* Carbohydrate ID And Carbohydrate Present */
#define CYBLE_CGMS_GLMT_FLG_MEAL     (0x02u) /* Meal Present */
#define CYBLE_CGMS_GLMT_FLG_TH       (0x04u) /* Tester-Health Present */
#define CYBLE_CGMS_GLMT_FLG_EXER     (0x08u) /* Exercise Duration And Exercise Intensity Present */
#define CYBLE_CGMS_GLMT_FLG_MEDC     (0x10u) /* Medication ID And Medication Present */
#define CYBLE_CGMS_GLMT_FLG_MVU      (0x20u) /* Medication Value Units 1 = liters, 0 = kilograms */
#define CYBLE_CGMS_GLMT_FLG_HBA1C    (0x40u) /* HbA1c Present */
#define CYBLE_CGMS_GLMT_FLG_EXFLAG   (0x80u) /* Extended Flags Present */

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
    CYBLE_CGMS_GLMT_TYPE_CWB = 1u,   /* Capillary Whole blood */
    CYBLE_CGMS_GLMT_TYPE_CPL,        /* Capillary Plasma */
    CYBLE_CGMS_GLMT_TYPE_VWB,        /* Venous Whole blood */
    CYBLE_CGMS_GLMT_TYPE_VPL,        /* Venous Plasma */
    CYBLE_CGMS_GLMT_TYPE_AWB,        /* Arterial Whole blood */
    CYBLE_CGMS_GLMT_TYPE_APL,        /* Arterial Plasma */
    CYBLE_CGMS_GLMT_TYPE_UWB,        /* Undetermined Whole blood */
    CYBLE_CGMS_GLMT_TYPE_UPL,        /* Undetermined Plasma */
    CYBLE_CGMS_GLMT_TYPE_ISF,        /* Interstitial Fluid */
    CYBLE_CGMS_GLMT_TYPE_CS          /* Control Solution */
}CYBLE_CGMS_CGMT_TYPE_T;

typedef enum
{
    CYBLE_CGMS_GLMT_SL_FR = 1u,      /* Finger */
    CYBLE_CGMS_GLMT_SL_AST,          /* Alternate Site Test */
    CYBLE_CGMS_GLMT_SL_EL,           /* Earlobe */
    CYBLE_CGMS_GLMT_SL_CS,           /* Control Solution */
    CYBLE_CGMS_GLMT_SL_NAV = 0x0Fu   /* Sample Location value not available */
}CYBLE_CGMS_GLMT_SL_T;

typedef struct
{
    uint8  flags;
    sfloat gluConc;     /* CGM Glucose Concentration */
    uint16 timeOffset;  /* in minutes */
    uint32 ssa;         /* Sensor Status Annunciation */
    sfloat trend;       /* CGM Trend Information */
    sfloat quality;     /* CGM Quality */
}CYBLE_CGMS_CGMT_T;


#define CYBLE_CGMS_CGFT_FTR_CL (0x00000001u) /* Calibration Supported */
#define CYBLE_CGMS_CGFT_FTR_PT (0x00000002u) /* Patient High/Low Alerts supported */
#define CYBLE_CGMS_CGFT_FTR_HO (0x00000004u) /* Hypo Alerts supported */
#define CYBLE_CGMS_CGFT_FTR_HR (0x00000008u) /* Hyper Alerts supported */
#define CYBLE_CGMS_CGFT_FTR_ID (0x00000010u) /* Rate of Increase/Decrease Alerts supported */
#define CYBLE_CGMS_CGFT_FTR_DS (0x00000020u) /* Device Specific Alert supported */
#define CYBLE_CGMS_CGFT_FTR_SM (0x00000040u) /* Sensor Malfunction Detection supported */
#define CYBLE_CGMS_CGFT_FTR_ST (0x00000080u) /* Sensor Temperature High-Low Detection supported */

#define CYBLE_CGMS_CGFT_FTR_HL (0x00000100u) /* Sensor Result High-Low Detection supported */
#define CYBLE_CGMS_CGFT_FTR_BL (0x00000200u) /* Low Battery Detection supported */
#define CYBLE_CGMS_CGFT_FTR_TE (0x00000400u) /* Sensor Type Error Detection supported */
#define CYBLE_CGMS_CGFT_FTR_DF (0x00000800u) /* General Device Fault supported */
#define CYBLE_CGMS_CGFT_FTR_EC (0x00001000u) /* E2E-CRC supported */
#define CYBLE_CGMS_CGFT_FTR_MB (0x00002000u) /* Multiple Bond supported */
#define CYBLE_CGMS_CGFT_FTR_MS (0x00004000u) /* Multiple Sessions supported */
#define CYBLE_CGMS_CGFT_FTR_TI (0x00008000u) /* CGM Trend Information supported */

#define CYBLE_CGMS_CGFT_FTR_QA (0x00010000u) /* CGM Quality supported */

#define CYBLE_CGMS_CGFT_TYPE_MASK    (0x0Fu) /* "Type" mask in the "tnsl" field */
#define CYBLE_CGMS_CGFT_SL_MASK      (0xF0u) /* "Sample Location" mask in the "tnsl" field */
#define CYBLE_CGMS_CGFT_SL_SHIFT     (4u)    /* "Sample Location" offset in the "tnsl" field */


/* CGM Feature characteristic structure */
typedef struct
{
    uint32 feature;     /* CGM Feature, actual size - 24 bit */
    uint8  type;        /* CGM Type, actual size - 4 bit */
    uint8  sampLoc;     /* CGM Sample Location, actual size - 4 bit */
}CYBLE_CGMS_CGFT_T;

#define CYBLE_SSTM_LEN (11u)

/* Time Zone */
typedef enum
{
    CYBLE_TIME_ZONE_M1200 = -48, /* UTC-12:00 */
    CYBLE_TIME_ZONE_M1100 = -44, /* UTC-11:00 */
    CYBLE_TIME_ZONE_M1000 = -40, /* UTC-10:00 */
    CYBLE_TIME_ZONE_M0930 = -38, /* UTC-9:30 */
    CYBLE_TIME_ZONE_M0900 = -36, /* UTC-9:00 */
    CYBLE_TIME_ZONE_M0800 = -32, /* UTC-8:00 */
    CYBLE_TIME_ZONE_M0700 = -28, /* UTC-7:00 */
    CYBLE_TIME_ZONE_M0600 = -24, /* UTC-6:00 */
    CYBLE_TIME_ZONE_M0500 = -20, /* UTC-5:00 */
    CYBLE_TIME_ZONE_M0430 = -18, /* UTC-4:30 */
    CYBLE_TIME_ZONE_M0400 = -16, /* UTC-4:00 */
    CYBLE_TIME_ZONE_M0330 = -14, /* UTC-3:30 */
    CYBLE_TIME_ZONE_M0300 = -12, /* UTC-3:00 */
    CYBLE_TIME_ZONE_M0200 = -8,  /* UTC-2:00 */
    CYBLE_TIME_ZONE_M0100 = -4,  /* UTC-1:00 */
    CYBLE_TIME_ZONE_ZERO  = 0,   /* UTC+0:00 */
    CYBLE_TIME_ZONE_P0100 = 4,   /* UTC+1:00 */
    CYBLE_TIME_ZONE_P0200 = 8,   /* UTC+2:00 */
    CYBLE_TIME_ZONE_P0300 = 12,  /* UTC+3:00 */
    CYBLE_TIME_ZONE_P0330 = 14,  /* UTC+3:30 */
    CYBLE_TIME_ZONE_P0400 = 16,  /* UTC+4:00 */
    CYBLE_TIME_ZONE_P0430 = 18,  /* UTC+4:30 */
    CYBLE_TIME_ZONE_P0500 = 20,  /* UTC+5:00 */
    CYBLE_TIME_ZONE_P0530 = 22,  /* UTC+5:30 */
    CYBLE_TIME_ZONE_P0545 = 23,  /* UTC+5:45 */
    CYBLE_TIME_ZONE_P0600 = 24,  /* UTC+6:00 */
    CYBLE_TIME_ZONE_P0630 = 26,  /* UTC+6:30 */
    CYBLE_TIME_ZONE_P0700 = 28,  /* UTC+7:00 */
    CYBLE_TIME_ZONE_P0800 = 32,  /* UTC+8:00 */
    CYBLE_TIME_ZONE_P0845 = 35,  /* UTC+8:45 */
    CYBLE_TIME_ZONE_P0900 = 36,  /* UTC+9:00 */
    CYBLE_TIME_ZONE_P0930 = 38,  /* UTC+9:30 */
    CYBLE_TIME_ZONE_P1000 = 40,  /* UTC+10:00 */
    CYBLE_TIME_ZONE_P1030 = 42,  /* UTC+10:30 */
    CYBLE_TIME_ZONE_P1100 = 44,  /* UTC+11:00 */
    CYBLE_TIME_ZONE_P1130 = 46,  /* UTC+11:30 */
    CYBLE_TIME_ZONE_P1200 = 48,  /* UTC+12:00 */
    CYBLE_TIME_ZONE_P1245 = 51,  /* UTC+12:45 */
    CYBLE_TIME_ZONE_P1300 = 52,  /* UTC+13:00 */
    CYBLE_TIME_ZONE_P1400 = 56   /* UTC+14:00 */
}CYBLE_TIME_ZONE_T;

#define CYBLE_TIME_ZONE_VAL_NUM (40u)


/* DST Offset */
typedef enum
{
    CYBLE_DSTOFFSET_ST = 0u,   /* Standard Time */
    CYBLE_DSTOFFSET_HF = 2u,   /* Half An Hour Daylight Time (+0.5h) */
    CYBLE_DSTOFFSET_DT = 4u,   /* Daylight Time (+1h) */
    CYBLE_DSTOFFSET_DD = 8u,   /* Double Daylight Time (+2h) */
}CYBLE_DSTOFFSET_T;

/* CGM Session Start Time characteristic structure */
typedef struct
{
    CYBLE_DATE_TIME_T sst;       /* Session Start Time */
    CYBLE_TIME_ZONE_T timeZone;  /* Time Zone */
    CYBLE_DSTOFFSET_T dstOffset; /* DST Offset */
}CYBLE_CGMS_SSTM_T;


/* Opcode of Record Access Control Point characteristic value type */
typedef enum
{
    CYBLE_CGMS_RACP_OPC_NA       = 0u,   /* Reserved for future use (Operator:N/A) */
    CYBLE_CGMS_RACP_OPC_REPORT_REC,      /* Report stored records (Operator: Value from Operator Table) */
    CYBLE_CGMS_RACP_OPC_DELETE_REC,      /* Delete stored records (Operator: Value from Operator Table) */
    CYBLE_CGMS_RACP_OPC_ABORT_OPN,       /* Abort operation (Operator: Null 'value of 0x00 from Operator Table') */
    CYBLE_CGMS_RACP_OPC_REPORT_NUM_REC,  /* Report number of stored records (Operator: Value from Operator Table) */
    CYBLE_CGMS_RACP_OPC_NUM_REC_RSP,     /* Number of stored records response (Operator: Null 'value of 0x00 from Operator Table') */
    CYBLE_CGMS_RACP_OPC_RSP_CODE,        /* Response Code (Operator: Null 'value of 0x00 from Operator Table') */
} CYBLE_CGMS_RACP_OPC_T;

/* Operator of Record Access Control Point characteristic value type */
typedef enum
{
    CYBLE_CGMS_RACP_OPR_NULL = 0u,  /* Null */
    CYBLE_CGMS_RACP_OPR_ALL,        /* All records */
    CYBLE_CGMS_RACP_OPR_LESS,       /* Less than or equal to */
    CYBLE_CGMS_RACP_OPR_GREAT,      /* Greater than or equal to */
    CYBLE_CGMS_RACP_OPR_WITHIN,     /* Within range of (inclusive) */
    CYBLE_CGMS_RACP_OPR_FIRST,      /* First record(i.e. oldest record) */
    CYBLE_CGMS_RACP_OPR_LAST        /* Last record (i.e. most recent record) */
} CYBLE_CGMS_RACP_OPR_T;

/* Operand of Record Access Control Point characteristic value type */
typedef enum
{
    CYBLE_CGMS_RACP_OPD_NA = 0u,     /* N/A */
    CYBLE_CGMS_RACP_OPD_1,           /* Filter parameters (as appropriate to Operator and Service) */
    CYBLE_CGMS_RACP_OPD_2,           /* Filter parameters (as appropriate to Operator and Service) */
    CYBLE_CGMS_RACP_OPD_NO_INCL,     /* Not included */
    CYBLE_CGMS_RACP_OPD_4,           /* Filter parameters (as appropriate to Operator and Service) */
    CYBLE_CGMS_RACP_OPD_NUM_REC,     /* Number of Records (Field size defined per service) */
    CYBLE_CGMS_RACP_OPD_RSP,         /* Request Op Code, Response Code Value */
    
} CYBLE_CGMS_RACP_OPD_T;

/* Operand Response Code Values of Record Access Control Point characteristic value type */
typedef enum
{
    CYBLE_CGMS_RACP_RSP_NA = 0u,     /* N/A */
    CYBLE_CGMS_RACP_RSP_SUCCESS,     /* Normal response for successful operation */
    CYBLE_CGMS_RACP_RSP_UNSPRT_OPC,  /* Normal response if unsupported Op Code is received */
    CYBLE_CGMS_RACP_RSP_INV_OPR,     /* Normal response if Operator received does not meet 
                                     * requirements of service (e.g. Null was expected) 
                                     */
    CYBLE_CGMS_RACP_RSP_UNSPRT_OPR,  /* Normal response if unsupported Operator is received */
    CYBLE_CGMS_RACP_RSP_INV_OPD,     /* Normal response if Operand received does not meet 
                                     * requirements of service 
                                     */
    CYBLE_CGMS_RACP_RSP_NO_REC,      /* Normal response if request to report stored records
                                     * or request to delete stored records resulted in no
                                     * records meeting criterion. 
                                     */
    CYBLE_CGMS_RACP_RSP_UNSUCCESS,   /* Normal response if request for Abort cannot be completed */
    CYBLE_CGMS_RACP_RSP_NO_COMPL,    /* Normal response if request for Abort cannot be completed */
    CYBLE_CGMS_RACP_RSP_UNSPRT_OPD   /* Normal response if unsupported Operand is received */
    
} CYBLE_CGMS_RACP_RSP_T;


typedef struct
{
    sfloat hal;       /* High Alert Level */
    sfloat lal;       /* Low Alert Level */
    sfloat hpo;       /* Hypo Alert Level */
    sfloat hpr;       /* Hyper Alert Level */
    sfloat dec;       /* Rate of Decrease Alert Level */
    sfloat inc;       /* Rate of Increase Alert Level */
}CYBLE_CGMS_ALRT_T;

/* Op Code - Response Codes of CGM Specific Ops Control Point characteristic value type */
typedef enum
{
	CYBLE_CGMS_SOCP_RSP_RSRV       = 0u, /* Reserved for Future Use. N/A. */
	CYBLE_CGMS_SOCP_RSP_SUCCESS    = 1u, /* Success. Normal response for successful operation. . */
	CYBLE_CGMS_SOCP_RSP_UNSPRT_OPC = 2u, /* Op Code not supported. Normal response if unsupported Op Code is received.. */
	CYBLE_CGMS_SOCP_RSP_INV_OPD    = 3u, /* Invalid Operand. Normal response if Operand received does not meet requirements of service.. */
	CYBLE_CGMS_SOCP_RSP_NO_COMPL   = 4u, /* Procedure not completed. Normal response if unable to complete procedure for any reason.. */
	CYBLE_CGMS_SOCP_RSP_POOR       = 5u  /* Parameter out of range. Normal response if Operand received does not meet range requirements . */
} CYBLE_CGMS_SOCP_RSP_T;

/* Op Code of CGM Specific Ops Control Point characteristic value type */
typedef enum
{
    CYBLE_CGMS_SOCP_OPC_NA   = 0u, /* Reserved for future use. N/A. */
    CYBLE_CGMS_SOCP_OPC_SINT = 1u, /* Set CGM Communication Interval. Response to this control point is Response Code (Op Code 0x0F). */
    CYBLE_CGMS_SOCP_OPC_GINT = 2u, /* Get CGM Communication Interval. Normal response to this control point is Op Code 0x03. For error conditions, response is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_RINT = 3u, /* CGM Communication Interval response. This is normal response to Op Code 0x02. */
    CYBLE_CGMS_SOCP_OPC_SGCV = 4u, /* Set Glucose Calibration Value. Response to this control point is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_GGCV = 5u, /* Get Glucose Calibration Value. Normal response to this control point is Op Code 0x06. for error conditions, response is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_RGCV = 6u, /* Glucose Calibration Value response. Glucose Calibration Value response. */
    CYBLE_CGMS_SOCP_OPC_SHAL = 7u, /* Set Patient High Alert Level. Response to this control point is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_GHAL = 8u, /* Get Patient High Alert Level. Normal response to this control point is Op Code 0x09. For error conditions, response is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_RHAL = 9u, /* Patient High Alert Level Response. This is normal response to Op Code 0x08. */
    CYBLE_CGMS_SOCP_OPC_SLAL = 10u, /* Set Patient Low Alert Level. Response to this control point is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_GLAL = 11u, /* Get Patient Low Alert Level. Normal response to this control point is Op Code 0x0C. Response is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_RLAL = 12u, /* Patient Low Alert Level Response. This is normal response to Op Code 0x0B. */
    CYBLE_CGMS_SOCP_OPC_SHPO = 13u, /* Set Hypo Alert Level. Response to this control point is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_GHPO = 14u, /* Get Hypo Alert Level. Normal response to this control point is Op Code 0x0F. Response is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_RHPO = 15u, /* Hypo Alert Level Response. This is normal response to Op Code 0x0E. */
    CYBLE_CGMS_SOCP_OPC_SHPR = 16u, /* Set Hyper Alert Level. Response to this control point is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_GHPR = 17u, /* Get Hyper Alert Level. Normal response to this control point is Op Code 0x12. Response is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_RHPR = 18u, /* Hyper Alert Level Response. This is normal response to Op Code 0x11. */
    CYBLE_CGMS_SOCP_OPC_SDEC = 19u, /* Set Rate of Decrease Alert Level. Response to this control point is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_GDEC = 20u, /* Get Rate of Decrease Alert Level. Normal response to this control point is Op Code 0x15. For error conditions, response is Response Code. */
    CYBLE_CGMS_SOCP_OPC_RDEC = 21u, /* Rate of Decrease Alert Level Response. This is normal response to Op Code 0x14. */
    CYBLE_CGMS_SOCP_OPC_SINC = 22u, /* Set Rate of Increase Alert Level. Response to this control point is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_GINC = 23u, /* Get Rate of Increase Alert Level. Normal response to this control point is Op Code 0x18. For error conditions, response is Response Code. */
    CYBLE_CGMS_SOCP_OPC_RINC = 24u, /* Rate of Increase Alert Level Response. This is normal response to Op Code 0x17. */
    CYBLE_CGMS_SOCP_OPC_RDSA = 25u, /* Reset Device Specific Alert. Response to this control point is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_STSN = 26u, /* Start the Session. Response to this control point is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_SPSN = 27u, /* Stop the Session. Response to this control point is defined in Op Code - Response Codes field. */
    CYBLE_CGMS_SOCP_OPC_RSPC = 28u  /* Response Code. see Op Code - Response Codes field. */
} CYBLE_CGMS_SOCP_OPC_T;

#define CYBLE_CGMS_SOCP_OPC_SN (1u) /* Length of the Start/Stop Session SOCP commands. */
#define CYBLE_CGMS_SOCP_OPC_IN (2u) /* Length of the Set CGM Communication Interval SOCP command. */
#define CYBLE_CGMS_SOCP_OPC_AL (3u) /* Length of the Set Alert Level (Patient High/Low, Hypo, Hyper, Rate of Decrease/Increase) SOCP commands. */
#define CYBLE_CGMS_SOCP_OPC_CV (11u) /* Length of the Set Glucose Calibration Value SOCP command. */


/***************************************
*      API function prototypes
***************************************/
uint16 CgmsCrc(uint8 length, uint8 *dataPtr);
uint8 CgmsCrcLength(uint8 length, uint8* dataPtr);
CYBLE_GATT_ERR_CODE_T CgmsCrcCheck(uint16 attrSize, CYBLE_GATT_VALUE_T *gattValue);
void CgmsPrintCharName(CYBLE_CGMS_CHAR_INDEX_T charIndex);
void CgmsInit(void);
void CgmsCallBack(uint32 event, void* eventParam);
void CgmsProcess(void);
void CgmsSendCgmtNtf(CYBLE_CGMS_CGMT_T cgmt);


/***************************************
*      External data references
***************************************/
extern uint8 attr[24u];
extern CYBLE_CGMS_CGMT_T cgmt[];
extern CYBLE_CGMS_CGFT_T cgft;

extern CYBLE_GAP_BONDED_DEV_ADDR_LIST_T bdList;
extern CYBLE_GAP_BD_ADDR_T peerBdAddr;
extern uint8 unbond;
extern uint8 bmcp;




#endif /* CGMSS_H  */

/* [] END OF FILE */
